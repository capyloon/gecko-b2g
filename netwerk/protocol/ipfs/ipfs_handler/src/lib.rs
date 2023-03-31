/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#[macro_use]
extern crate xpcom;

use cid::{multibase, Cid};
use log::debug;
use nserror::{nsresult, NS_ERROR_FAILURE, NS_OK};
use nsstring::{nsACString, nsCString};
use std::convert::TryFrom;
use std::ffi::CString;
use std::ops::DerefMut;
use std::ptr;
use xpcom::{
    getter_addrefs,
    interfaces::{
        nsIChannel, nsIIOService, nsILoadInfo, nsIPrefService, nsIProtocolHandler, nsIURI,
    },
    RefPtr,
};

#[derive(Debug, PartialEq)]
enum Protocol {
    Ipfs,
    Ipns,
    Tile,
}

impl Protocol {
    fn as_nscstring(&self) -> nsCString {
        match &self {
            Protocol::Ipfs => nsCString::from("ipfs"),
            Protocol::Ipns => nsCString::from("ipns"),
            Protocol::Tile => nsCString::from("tile"),
        }
    }
}

// Default Unix Domain Socket path.
// Use the 'ipfs.uds_path' preference to change it.
#[cfg(not(target_os = "android"))]
static DEFAULT_UDS_PATH: &str = "/tmp/ipfsd.http";
#[cfg(target_os = "android")]
static DEFAULT_UDS_PATH: &str = "/dev/socket/ipfsd.http";

// Helper functions to get a char pref with a default value.
fn fallible_get_char_pref(name: &str, default_value: &str) -> Result<nsCString, nsresult> {
    if let Ok(pref_service) = xpcom::components::Preferences::service::<nsIPrefService>() {
        let branch = xpcom::getter_addrefs(|p| {
            // Safe because:
            //  * `null` is explicitly allowed per documentation
            //  * `p` is a valid outparam guaranteed by `getter_addrefs`
            unsafe { pref_service.GetBranch(std::ptr::null(), p) }
        })?;
        let pref_name = CString::new(name).map_err(|_| NS_ERROR_FAILURE)?;
        let mut pref_value = nsCString::new();
        // Safe because:
        //  * `branch` is non-null (otherwise `getter_addrefs` would've been `Err`
        //  * `pref_name` exists so a pointer to it is valid for the life of the function
        //  * `channel` exists so a pointer to it is valid, and it can be written to
        unsafe {
            if (*branch)
                .GetCharPref(
                    pref_name.as_ptr(),
                    pref_value.deref_mut() as *mut nsACString,
                )
                .to_result()
                .is_err()
            {
                pref_value = default_value.into();
            }
        }
        Ok(nsCString::from(pref_value))
    } else {
        Ok(nsCString::from(default_value))
    }
}

fn get_char_pref(name: &str, default_value: &str) -> nsCString {
    match fallible_get_char_pref(name, default_value) {
        Ok(value) => value,
        Err(_) => nsCString::from(default_value),
    }
}

#[xpcom(implement(nsIProtocolHandler), atomic)]
struct IpfsHandler {
    // The handler protocol.
    protocol: Protocol,
    uds_path: String,
}

impl IpfsHandler {
    fn new(protocol: Protocol) -> RefPtr<Self> {
        debug!("IpfsHandler::new {:?}", protocol);

        let uds_path = get_char_pref("ipfs.uds_path", DEFAULT_UDS_PATH);
        Self::allocate(InitIpfsHandler {
            uds_path: uds_path.to_utf8().replace("/", "%2F"),
            protocol,
        })
    }

    xpcom_method!(allow_port => AllowPort(port: i32, scheme: *const libc::c_char, _retval: *mut bool));
    fn allow_port(
        &self,
        _port: i32,
        _scheme: *const libc::c_char,
        retval: *mut bool,
    ) -> Result<(), nsresult> {
        unsafe {
            (*retval) = false;
        }
        Ok(())
    }

    xpcom_method!(get_scheme => GetScheme(aScheme: *mut nsACString));
    fn get_scheme(&self, scheme: *mut nsACString) -> Result<(), nsresult> {
        unsafe {
            (*scheme).assign(&self.protocol.as_nscstring());
        }

        Ok(())
    }

    xpcom_method!(new_channel => NewChannel(aURI: *const nsIURI, aLoadinfo: *const nsILoadInfo, retval: *mut *const nsIChannel));
    fn new_channel(
        &self,
        uri: *const nsIURI,
        load_info: *const nsILoadInfo,
        retval: *mut *const nsIChannel,
    ) -> Result<(), nsresult> {
        debug!("new_channel {:?}", {
            let mut spec = nsCString::new();
            unsafe {
                (*uri).GetSpec(&mut *spec);
            }
            spec
        });

        let mut host = nsCString::new();
        let mut path_query = nsCString::new();
        let scheme = self.protocol.as_nscstring();

        unsafe {
            (*uri).GetHost(&mut *host);
            (*uri).GetPathQueryRef(&mut *path_query);
        }

        // Mapping to UDS endpoint:
        // ipfs://bafybeiemxf5abjwjbikoz4mc3a3dla6ual3jsgpdr4cjr3oz3evfyavhwq/wiki/Vincent_van_Gogh.html ->
        // http+unix://<uds path>/ipfs/bafybeiemxf5abjwjbikoz4mc3a3dla6ual3jsgpdr4cjr3oz3evfyavhwq/wiki/Vincent_van_Gogh.html

        let url_path = match self.protocol {
            Protocol::Ipfs | Protocol::Tile => {
                // Special case when host is "localhost" and the method is POST: url_path is empty.
                // TODO: figure out the method used, but it's not available from nsILoadInfo
                // See: https://searchfox.org/mozilla-central/rev/aa329cf7506ddd966542e642ec00223fd7461599/dom/fetch/FetchDriver.cpp#706-709
                if host == "localhost" {
                    String::new()
                } else {
                    // Try to convert 'host' into a CIDv1
                    let cid =
                        Cid::try_from(host.to_utf8().as_ref()).map_err(|_| NS_ERROR_FAILURE)?;
                    // Same as Cid::to_string_v1() which is unfortunately private.
                    multibase::encode(multibase::Base::Base32Lower, cid.to_bytes().as_slice())
                }
            }
            Protocol::Ipns => {
                // For ipns://, just use the host.
                host.to_string()
            }
        };

        let uds_url = if url_path.is_empty() {
            format!("http+unix://{}/{}/", self.uds_path, scheme)
        } else {
            format!(
                "http+unix://{}/{}/{}{}",
                self.uds_path, scheme, url_path, path_query
            )
        };

        // println!("IPFS: UDS url is {}", uds_url);

        let io_service =
            xpcom::components::IO::service::<nsIIOService>().map_err(|_| NS_ERROR_FAILURE)?;
        unsafe {
            let final_uri: RefPtr<nsIURI> = getter_addrefs(|p| {
                io_service.NewURI(&*nsCString::from(uds_url), ptr::null(), ptr::null(), p)
            })?;

            io_service.NewChannelFromURIWithLoadInfo(&*final_uri, load_info, retval);
            (*retval)
                .as_ref()
                .ok_or(NS_ERROR_FAILURE)?
                .SetOriginalURI(uri);
        }

        Ok(())
    }
}

#[no_mangle]
pub unsafe extern "C" fn ipfs_construct(result: &mut *const nsIProtocolHandler) {
    let inst = IpfsHandler::new(Protocol::Ipfs);
    *result = inst.coerce::<nsIProtocolHandler>();
    std::mem::forget(inst);
}

#[no_mangle]
pub unsafe extern "C" fn ipns_construct(result: &mut *const nsIProtocolHandler) {
    let inst = IpfsHandler::new(Protocol::Ipns);
    *result = inst.coerce::<nsIProtocolHandler>();
    std::mem::forget(inst);
}

#[no_mangle]
pub unsafe extern "C" fn tile_construct(result: &mut *const nsIProtocolHandler) {
    let inst = IpfsHandler::new(Protocol::Tile);
    *result = inst.coerce::<nsIProtocolHandler>();
    std::mem::forget(inst);
}
