# Project 10 Additions

The program can now:

Map path names to inodes using namei()
Return the root inode when given "/"
Find directories in the root directory using namei()
Create new directories using directory_make()
Create . and .. entries for new directories
Add new directory entries to the parent directory
Update parent directory sizes when new directories are created
Reject invalid or unsupported directory paths
Support opening newly-created directories through namei()