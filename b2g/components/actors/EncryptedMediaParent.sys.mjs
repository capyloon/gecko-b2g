/* vim: set ts=2 sw=2 sts=2 et tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import { XPCOMUtils } from "resource://gre/modules/XPCOMUtils.sys.mjs";

export class EncryptedMediaParent extends JSWindowActorParent {
  isUiEnabled() {
    // Always show the UI on b2g.
    return true;
  }

  ensureEMEEnabled(aBrowser, aKeySystem) {
    Services.prefs.setBoolPref("media.eme.enabled", true);
    if (
      aKeySystem &&
      aKeySystem == "com.widevine.alpha" &&
      Services.prefs.getPrefType("media.gmp-widevinecdm.enabled") &&
      !Services.prefs.getBoolPref("media.gmp-widevinecdm.enabled")
    ) {
      Services.prefs.setBoolPref("media.gmp-widevinecdm.enabled", true);
    } else {
      console.error(`EME: ensureEMEEnabled failed for ${aKeySystem}`);
    }
    aBrowser.reload();
  }

  isKeySystemVisible(aKeySystem) {
    if (!aKeySystem) {
      return false;
    }
    if (
      aKeySystem == "com.widevine.alpha" &&
      Services.prefs.getPrefType("media.gmp-widevinecdm.visible")
    ) {
      return Services.prefs.getBoolPref("media.gmp-widevinecdm.visible");
    }
    return true;
  }

  receiveMessage(aMessage) {
    // The top level browsing context's embedding element should be a xul browser element.
    let browser = this.browsingContext.top.embedderElement;

    if (!browser) {
      // We don't have a browser so bail!
      return;
    }

    let parsedData;
    try {
      parsedData = JSON.parse(aMessage.data);
    } catch (ex) {
      console.error("Malformed EME video message with data: ", aMessage.data);
      return;
    }
    let { status, keySystem } = parsedData;

    // First, see if we need to do updates. We don't need to do anything for
    // hidden keysystems:
    if (!this.isKeySystemVisible(keySystem)) {
      return;
    }
    if (status == "cdm-not-installed") {
      Services.obs.notifyObservers(browser, "EMEVideo:CDMMissing");
    }

    // Don't need to show UI if disabled.
    if (!this.isUiEnabled()) {
      return;
    }

    let notificationId;
    let callback;
    switch (status) {
      case "available":
      case "cdm-created":
        // Only show the chain icon for proprietary CDMs. Clearkey is not one.
        if (keySystem != "org.w3.clearkey") {
          this.showPopupNotificationForSuccess(browser, keySystem);
        }
        // ... and bail!
        return;

      case "api-disabled":
      case "cdm-disabled":
        notificationId = "drm-content-disabled";
        callback = () => {
          console.log(`EME before ensureEMEEnabled`);
          this.ensureEMEEnabled(browser, keySystem);
        };
        break;

      case "cdm-not-installed":
        notificationId = "drm-content-cdm-installing";
        break;

      case "cdm-not-supported":
        // Not to pop up user-level notification because they cannot do anything
        // about it.
        return;
      default:
        console.error(
          new Error(
            "Unknown message ('" +
              status +
              "') dealing with EME key request: " +
              aMessage.data
          )
        );
        return;
    }

    // Relay the notificatino to the embedder
    Services.obs.notifyObservers(
      { notificationId, callback },
      "web-embedder-eme"
    );
  }

  async showPopupNotificationForSuccess(aBrowser) {
    Services.obs.notifyObservers(
      { successNotification: true },
      "web-embedder-eme"
    );
  }
}
