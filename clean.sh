echo "THIS SCRIPT IRREVERSIBLY DELETES FILES YOU MAY WANT"
echo "DO NOT RUN IT UNLESS YOU ABSOLUTELY KNOW WHAT YOU ARE DOING"
exit 1

rm -rf 'Program Files/Common Files/Microsoft Shared/Stationery'
rm -rf 'Program Files/Common Files/services'
rm -rf 'Program Files/Online Services'
rm -rf 'Program Files/Uninstall Information'

rm -rf recycled
rm -f videorom.bin

rm -rf windows/applog
rm -rf windows/Cookies
rm -rf 'windows/Desktop/Online Services'
rm -rf 'windows/Downloaded Program Files'
rm -rf windows/Favorites
rm -rf windows/History
rm -rf 'windows/Offline Web Pages'
rm -rf windows/Recent
rm -rf windows/samples
rm -rf windows/ShellIconCache
rm -rf 'windows/Start Menu/Programs/Online Services'
rm -rf windows/sysbckup
rm -rf windows/temp
rm -rf 'windows/Temporary Internet Files'
rm -rf windows/ttfCache

find -iname '*.---' -delete
find -iname '*.bak' -delete
find -iname '*.prv' -delete
find -iname '*log.dat' -delete
find -iname '*log.txt' -delete
find windows/inf -iname '*.pnf' -delete

rm -f 'windows/Exit To Dos.pif'
rm -f windows/inf/{drvdata,drvidx}.bin
# dangerous, only uncomment if you know your backup is good!
# rm -f windows/{user,system}.dat
rm -f windows/win386.swp

touch -d "2000-01-01 00:00 UTC" windows/win386.swp
