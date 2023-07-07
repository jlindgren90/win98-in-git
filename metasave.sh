rm mattrib.sh
rm meta.sh
cd /
umount /c
mattrib -p -/ c: | sed -e 's/mattrib -a/mattrib/' -e '/mattrib "/d' | LC_ALL=C sort > /tmp/mattrib.sh
mount /c
cd /c
find -type d -empty -printf 'mkdir -p "%p"\n' | LC_ALL=C sort > /tmp/meta.sh
find -type f -printf 'chmod %m "%p" && touch -d@%T@ "%p"\n' | LC_ALL=C sort >> /tmp/meta.sh
cp /tmp/mattrib.sh /c/
cp /tmp/meta.sh /c/
