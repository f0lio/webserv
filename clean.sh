while true; do
	read -p "Do you wish to rm files specified in .gitignore? " yn
	case $yn in
		[Yy]* ) break;;
		[Nn]* ) exit;;
		* ) echo "yes for yes or no for no";;
	esac
done

for f in $(cat .gitignore | grep -v "#"); do
	rm -Rvf $f 2>/dev/null
done
echo "mchaaaw"
