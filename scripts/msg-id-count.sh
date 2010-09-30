cat new.out | sed 's/msg=[ ]*/msg=/' | tr ' ' '\n' | grep msg= | cut -d '=' -f 2 | uniq -c | sort -n
