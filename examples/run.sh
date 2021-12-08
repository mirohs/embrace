name=`basename $1 .c`
name=`basename $name .d`
# echo $name
make $name && ./$name
