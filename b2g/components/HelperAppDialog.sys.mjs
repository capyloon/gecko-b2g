// -*- indent-tabs-mode: nil; js-indent-level: 2 -*-
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

const lazy = {};
ChromeUtils.defineESModuleGetters(lazy, {
  Downloads: "resource://gre/modules/Downloads.sys.mjs",
});

// -----------------------------------------------------------------------
// HelperApp Launcher Dialog
//
// For now on b2g we never prompt and just download to the default
// location.
//
// -----------------------------------------------------------------------

export function HelperAppLauncherDialog() {}

HelperAppLauncherDialog.prototype = {
  classID: Components.ID("{710322af-e6ae-4b0c-b2c9-1474a87b077e}"),
  QueryInterface: ChromeUtils.generateQI([Ci.nsIHelperAppLauncherDialog]),

  show(aLauncher, aContext, aReason) {
    aLauncher.MIMEInfo.preferredAction = Ci.nsIMIMEInfo.saveToDisk;
    aLauncher.promptForSaveDestination();
  },

  promptForSaveToFileAsync(
    aLauncher,
    aContext,
    aDefaultFile,
    aSuggestedFileExt,
    aForcePrompt
  ) {
    // Retrieve the user's default download directory.
    (async function () {
      let file = null;
      try {
        let defaultFolder =
          await lazy.Downloads.getPreferredDownloadsDirectory();
        let dir = Cc["@mozilla.org/file/local;1"].createInstance(Ci.nsIFile);
        dir.initWithPath(defaultFolder);
        file = this.validateLeafName(dir, aDefaultFile, aSuggestedFileExt);
      } catch (e) {}
      aLauncher.saveDestinationAvailable(file);
    })
      .bind(this)()
      .then(null, console.error);
  },

  validateLeafName(aLocalFile, aLeafName, aFileExt) {
    if (!(aLocalFile && this.isUsableDirectory(aLocalFile))) {
      return null;
    }

    // Remove any leading periods, since we don't want to save hidden files
    // automatically.
    aLeafName = aLeafName.replace(/^\.+/, "");

    if (aLeafName == "") {
      aLeafName = "unnamed" + (aFileExt ? "." + aFileExt : "");
    }
    aLocalFile.append(aLeafName);

    this.makeFileUnique(aLocalFile);
    return aLocalFile;
  },

  makeFileUnique(aLocalFile) {
    try {
      // Note - this code is identical to that in
      //   toolkit/content/contentAreaUtils.js.
      // If you are updating this code, update that code too! We can't share code
      // here since this is called in a js component.
      let collisionCount = 0;
      while (aLocalFile.exists()) {
        collisionCount++;
        if (collisionCount == 1) {
          // Append "(2)" before the last dot in (or at the end of) the filename
          // special case .ext.gz etc files so we don't wind up with .tar(2).gz
          if (aLocalFile.leafName.match(/\.[^\.]{1,3}\.(gz|bz2|Z)$/i)) {
            aLocalFile.leafName = aLocalFile.leafName.replace(
              /\.[^\.]{1,3}\.(gz|bz2|Z)$/i,
              "(2)$&"
            );
          } else {
            aLocalFile.leafName = aLocalFile.leafName.replace(
              /(\.[^\.]*)?$/,
              "(2)$&"
            );
          }
        } else {
          // replace the last (n) in the filename with (n+1)
          aLocalFile.leafName = aLocalFile.leafName.replace(
            /^(.*\()\d+\)/,
            "$1" + (collisionCount + 1) + ")"
          );
        }
      }
      aLocalFile.create(Ci.nsIFile.NORMAL_FILE_TYPE, 0o600);
    } catch (e) {
      dump("*** exception in makeFileUnique: " + e + "\n");

      if (e.result == Cr.NS_ERROR_FILE_ACCESS_DENIED) {
        throw e;
      }

      if (aLocalFile.leafName == "" || aLocalFile.isDirectory()) {
        aLocalFile.append("unnamed");
        if (aLocalFile.exists()) {
          aLocalFile.createUnique(Ci.nsIFile.NORMAL_FILE_TYPE, 0o600);
        }
      }
    }
  },

  isUsableDirectory(aDirectory) {
    return (
      aDirectory.exists() && aDirectory.isDirectory() && aDirectory.isWritable()
    );
  },
};
