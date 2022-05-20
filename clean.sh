while [$1 != "-y"]; do
	read -p "Do you wish to rm files specified in .gitignore? " yn
	case $yn in
		[Yy]* ) break;;
		[Nn]* ) exit;;
		* ) echo "    - (y)es for yes\n    - (n)o for no";;
	esac
done

for f in $(cat .gitignore | grep -v "#"); do
	rm -Rvf $f 2>/dev/null
done
echo "\nmchaaaw"
