/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 * All functions on Directory that accept DOMString arguments for file or
 * directory names only allow relative path to current directory itself. The
 * path should be a descendent path like "path/to/file.txt" and not contain a
 * segment of ".." or ".". So the paths aren't allowed to walk up the directory
 * tree. For example, paths like "../foo", "..", "/foo/bar" or "foo/../bar" are
 * not allowed.
 *
 * http://w3c.github.io/filesystem-api/#idl-def-Directory
 * https://microsoftedge.github.io/directory-upload/proposal.html#directory-interface
 */

[Exposed=(Window,Worker)]
interface Directory {
  // This ChromeOnly constructor is used by the MockFilePicker for testing only.
  [Throws, ChromeOnly]
  constructor(DOMString path);

  /*
   * The leaf name of the directory.
   */
  [Throws]
  readonly attribute DOMString name;

  /*
   * Creates a new file or replaces an existing file with given data. The file
   * should be a descendent of current directory.
   *
   * @param path The relative path of the new file to current directory.
   * @param options It has two optional properties, 'ifExists' and 'data'.
   * If 'ifExists' is 'fail' and the path already exists, createFile must fail;
   * If 'ifExists' is 'replace', the path already exists, and is a file, create
   * a new file to replace the existing one;
   * If 'ifExists' is 'replace', the path already exists, but is a directory,
   * createFile must fail.
   * Otherwise, if no other error occurs, createFile will create a new file.
   * The 'data' property contains the new file's content.
   * @return If succeeds, the promise is resolved with the new created
   * File object. Otherwise, rejected with a DOM error.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<File> createFile(DOMString path, optional CreateFileOptions options = {});

  /*
   * Creates a descendent directory. This method will create any intermediate
   * directories specified by the path segments.
   *
   * @param path The relative path of the new directory to current directory.
   * If path exists, createDirectory must fail.
   * @return If succeeds, the promise is resolved with the new created
   * Directory object. Otherwise, rejected with a DOM error.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<Directory> createDirectory(DOMString path);

  /*
   * Gets a descendent file or directory with the given path.
   *
   * @param path The descendent entry's relative path to current directory.
   * @return If the path exists and no error occurs, the promise is resolved
   * with a File or Directory object, depending on the entry's type. Otherwise,
   * rejected with a DOM error.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<(File or Directory)> get(DOMString path);

  /*
   * Deletes a file or an empty directory. The target must be a descendent of
   * current directory.
   * @param path If a DOM string is passed, it is the relative path of the
   * target. Otherwise, the File or Directory object of the target should be
   * passed.
   * @return If the target is a non-empty directory, or if deleting the target
   * fails, the promise is rejected with a DOM error. If the target did not
   * exist, the promise is resolved with boolean false. If the target did exist
   * and was successfully deleted, the promise is resolved with boolean true.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<boolean> remove((DOMString or File or Directory) path);

  /*
   * Deletes a file or a directory recursively. The target should be a
   * descendent of current directory.
   * @param path If a DOM string is passed, it is the relative path of the
   * target. Otherwise, the File or Directory object of the target should be
   * passed.
   * @return If the target exists, but deleting the target fails, the promise is
   * rejected with a DOM error. If the target did not exist, the promise is
   * resolved with boolean false. If the target did exist and was successfully
   * deleted, the promise is resolved with boolean true.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<boolean> removeDeep((DOMString or File or Directory) path);

  /*
   * Copy files to the target directory. If source is a directory, will also
   * copy all its descendants to the target directory.
   * For default (targetStorage is unspecified or null in options), both source
   * and target(destination) must be descendants of caller, if DOMString is
   * passed, it is the relative path of caller.
   * If options.targetStorage is defined, target should be descendants of
   * options.targetStorage, if DOMString is passed, it is the relative path to
   * root of options.targetStorage.
   * @return If source and target exists, but copy has failed, the promise is
   * rejected with a DOM error(invalid path, target is not a directory...etc).
   * If source and target did not exist, the promise is resolved with boolean
   * false. If the target did exist and was successfully copied, the promise is
   * resolved with boolean true.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<boolean> copyTo((DOMString or File or Directory) source,
                          (DOMString or Directory) target,
                          optional CopyMoveOptions options = {});

  /*
   * Move files to the target directory. If source is a directory, will also
   * copy all its descendants to the target directory.
   * For default (targetStorage is unspecified or null in options), both source
   * and target(destination) must be descendants of caller, if DOMString is
   * passed, it is the relative path of caller.
   * If options.targetStorage is defined, target should be descendants of
   * options.targetStorage, if DOMString is passed, it is the relative path to
   * root of options.targetStorage.
   * @return If source and target exists, but move failed, the promise is
   * rejected with a DOM error(invalid path, target is not a directory...etc).
   * If source and target did not exist, the promise is resolved with boolean
   * false. If the target did exist and was successfully moved, the promise is
   * resolved with boolean true.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<boolean> moveTo((DOMString or File or Directory) source,
                          (DOMString or Directory) target,
                          optional CopyMoveOptions options = {});

  /*
   * Rename a file or a directory. The oldName should
   * be descendants of current directory.
   */
  [Func="mozilla::dom::Directory::DeviceStorageEnabled", NewObject]
  Promise<boolean> renameTo((DOMString or File or Directory) oldName,
                             DOMString newName);
};

[Exposed=(Window,Worker)]
partial interface Directory {
  // Already defined in the main interface declaration:
  //readonly attribute DOMString name;

  /*
   * The path of the Directory (includes both its basename and leafname).
   * The path begins with the name of the ancestor Directory that was
   * originally exposed to content (say via a directory picker) and traversed
   * to obtain this Directory.  Full filesystem paths are not exposed to
   * unprivilaged content.
   */
  [Throws]
  readonly attribute DOMString path;

  /*
   * Getter for the immediate children of this directory.
   */
  [NewObject]
  Promise<sequence<(File or Directory)>> getFilesAndDirectories();

  [NewObject]
  Promise<sequence<File>> getFiles(optional boolean recursiveFlag = false);
};

enum CreateIfExistsMode { "replace", "fail" };

dictionary CreateFileOptions {
  CreateIfExistsMode ifExists = "fail";
  (DOMString or Blob or ArrayBuffer or ArrayBufferView) data;
};

dictionary CopyMoveOptions {
  // See copyTo, moveTo for more information.
  DeviceStorage? targetStorage = null;

  // If true, and if file of same name already exists in target folder, will
  // keep both and auto append a suffix to source file. ex: xxx(1).txt, and
  // xxx(2).txt if xxx(1).txt exists...etc.
  boolean keepBoth = false;
};
