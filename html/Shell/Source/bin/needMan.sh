d=$(find "$(pwd)" -mindepth 1 -maxdepth 2 -type d '!' -exec test -e "{}/man" ';' -print)
for dir in $d;
do
	echo -n "$(basename $dir)  "
done
echo
