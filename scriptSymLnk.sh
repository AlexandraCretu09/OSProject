filename=$1

#chown -R owner:group filename

chmod ugo+rwx $filename
chmod g-x,o-rwx $filename