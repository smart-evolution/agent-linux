V=$(cat /dev/urandom | env LC_CTYPE=C tr -dc '0-9' | fold -w 8 | head -n 1)
touch ./id.h
echo "#define HARDWARE_ID \"$V\"" > ./id.h
