* Display all the entries in a path &check;
    * Order all entries (first all the folders, then the files; all in alphanumeric order) &check;
    * For now, display hidden entries as well; later, it will be a toggle &check;
    * Add navigation between paths &check;
* Add a search for paths &check;
    * Make sure the path is valid (e.g. no invalid characters, the path exists, etc) &check;
* Add buttons for forward (next folder), back (previous folder), up (parent folder) &check;, home (home folder) &check;, and refresh (same folder) &check;
    * The forward/back buttons should be disabled if there are no next/previous folders
    * The up button should be disabled if the current path is the root folder
    * The home button should be disabled if the current path is the home folder
* Add buttons for adding &check;, permanently deleting &check;, and trashing &check; files and folders
    * When adding entries, the name should be valid &check;
    * When permanently deleting folders, prompt the user for confirmation &check;
    * Use the trash command in order to trash entries &check;
* Finish trash folder implementation, adding the options to:
    * View trash folder
    * Restore files/folders to previous location
    * Remove (i.e. permanently delete) files/folders from trash folder
    * Empty (i.e. permanently delete all file/folders) from trash folder
* Add a searh bar for entries
* Add icons to differentiate files from folders
* Add icon for the window
* Ability to rename, move, copy, cut, and paste files/folder
* Make files/folders display as soon as they are loaded, instead of after all files/folders are loaded