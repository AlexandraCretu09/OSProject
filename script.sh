

filename=$1

gcc -Wall  $filename 2> error.log

num_warnings=$(grep -c warning error.log)
num_errors=$(grep -c error error.log)

echo "Number of warnings: $num_warnings"
echo "Number of errors: $num_errors"
