--------------------
WINDOWS 98 - IN GIT!
--------------------

WARNING: USE AT YOUR OWN RISK. I TAKE NO RESPONSIBILITY FOR ANY DATA (OR
HARDWARE) DESTROYED BY FOLLOWING THE STEPS IN THIS GUIDE.


** WHAT IS THIS? **

This is a project with absolutely no real-world application, for those of us
unfortunate enough to have a soft spot in our hearts for the hopelessly
antiquated, utterly insecure, DOS-based Windows 98. (It could probably also be
used with Windows 95 or even earlier versions, but I've only tested 98.)

Windows 98, on its own, makes no provision for system integrity. There are no
(enforced) file permissions, no real user IDs, and generally no protection
against careless users or carelessly written software trashing the system.
Furthermore, there is no reliable way to completely "roll back" a change to the
system: for example, to uninstall a program or driver and ensure that all traces
of it are really gone.

Later versions of Windows added some of these protections, along with a limited
System Restore feature that allows rolling back the entire system to a previous
point in time. But this is still limited: if you install A, B, and then C, you
cannot use System Restore to undo only the installation of A while leaving B and
C intact.

Imagine if the powerful concepts of Git could be applied to a Windows 98
installation. Want to try out a new piece of software for a few days without
leaving traces of it installed permanently? Create a branch. Uninstall a driver
from 2 years ago without affecting anything else? Revert that commit. Check what
autorun hooks were added to the registry by a sketchy piece of software? Simple
as "git diff".


** HOW DOES IT WORK? **

There are some hurdles to making it work:

- Getting modern Git to even run on Windows 98 would be a challenge, and even
  then, modifying system files within a running Windows system would be tricky,
  so I didn't try. Instead, this project requires dual-booting with Linux.
  Debian is good for this; Debian 12 (bookworm) runs happily on PCs from 20+
  years ago, if you are careful what you install. The Git history is stored
  safely off the C: drive, on the Linux drive/partition.

- The *#%^$@! registry. One massive binary file (well, two actually) in a
  proprietary format, containing crucial system settings and driver/hardware
  information require to boot successfully, alongside zillions of hex-coded
  "class IDs" and arbitrary state data dumped there by every program ever run on
  the machine. Git of course doesn't deal well with binary files like this, but
  there is a solution: Windows 98's regedit.exe can be run in DOS mode and
  translate the registry to/from a (relatively) human-readable text format,
  which is more amenable to version control operations (diff, merge, etc.)

- Time zones. Time zones are a long-standing annoyance when dual-booting Windows
  and Linux in general. Linux typically assumes the hardware clock is set to
  UTC, while Windows traditionally assumes it is in the local time zone. Later
  versions of Windows can be made to support UTC via a registry setting, but
  this is not (to my knowledge) possible with Windows 98. My approach is to
  leave the hardware clock set to UTC, tell Windows 98 not to adjust the clock
  for Daylight Saving Time, and accept that the time will just be displayed
  wrong in Windows. I don't connect Windows 98 to the Internet anyway (and you
  shouldn't either) so it doesn't matter much.

- Timestamps and DOS file attributes (read-only, hidden, system). These are
  important to preserve as part of an OS snapshot -- Windows will complain on
  boot if certain timestamps are wrong -- but Git doesn't store them. Timestamps
  and the read-only attribute are visible when the C: drive is mounted in Linux;
  I use (and include in this repository) shell scripts to save and restore
  those. The hidden and system attributes are trickier since they are not
  accessible through native Linux APIs, but the program "mattrib" (from the
  "mtools" package) can be used to save and restore these.


** SETUP GUIDE **

Prequisites:

I'll assume you already have both Windows 98 and a recent version of Debian
installed, and are an expert with Git.

1. Clone this repository *in bare format* (git clone --bare) to /c.git on
   the Linux drive/partition. /c.git should be owned by a non-root user.

2. Give the same non-root user read-write access to the block device for the
   Windows 98 partition (this is unfortunately necessary to run mattrib, which
   uses the unmounted block device directly). Then create ~/.mtoolsrc with the
   following contents (replacing /dev/sda1 with the correct block device):

     drive c: file="/dev/sda1"

3. Mount the Windows 98 partition (C: drive) to /c, with read-write permissions
   for the same user. I use the following /etc/fstab line:

     /dev/sda1 /c vfat noatime,user,noauto,shortname=lower,tz=UTC

4. Set up the needed Git environment variables:

     export GIT_DIR=/c.git
     export GIT_WORK_TREE=/c

Running "git status" should now show the files in this repository (README.txt
and the various *.sh and *.bat files) as deleted and all the files on the C:
drive (autoexec.bat, msdos.sys, the Program Files and Windows folders, etc.) as
untracked. If this does not match what you see, *STOP HERE* and figure out why
before continuing.

5. Create the initial commit:

   a. Use "git add" to stage all of the untracked files (except, I would
      suggest, windows/win386.swp) for commit.
   b. Use "git checkout" to create (un-delete) the files included in this
      repository.
   c. Run ". metasave.sh" to capture timestamps and DOS file attributes. This
      will create two executable scripts (meta.sh and mattrib.sh) that can be
      run later to restore the same timestamps and file attributes. Use
      "git add" to stage these two scripts for commit as well.
   d. Now windows/win386.swp should be the only untracked file. Check that this
      is the case and run "git commit".

6. While meta.sh is a valid shell script that can be invoked directly, it can be
   quite slow to run. I have included a small C utility which parses meta.sh and
   performs the same operations much more quickly. First check that $HOME/bin
   exists and is included in $PATH; then compile the utility as follows:

     gcc -Wall -O2 -o ~/bin/fastmetarestore fastmetarestore.c \
      `pkg-config --cflags --libs glib-2.0`

At this point, the /c.git repository is *in theory* a complete backup of the C:
drive. You can restore the full backup or individual files from it with the
appropriate "git checkout" commands. Running ". metarestore.sh" will restore
timestamps and DOS file attributes as well.

7. Converting the Windows registry to text format is optional but makes merges
   and reverts easier. To do this, simply start (or restart) Windows 98 in DOS
   mode and run the included batch file, regsave.bat. This will create the file
   windows/all.reg which contains the entire registry in textual format. To
   perform the reverse operation (text to binary), use regbuild.bat.

8. I have also included a clean.sh script which deletes temporary and (in my
   opinion) useless files that Windows 98 creates. I run this script before each
   commit since it helps to reduce the number of insignificant changes. However,
   I strongly recommend waiting until *after* the initial commit to run
   clean.sh, to avoid accidentally deleting anything important.
