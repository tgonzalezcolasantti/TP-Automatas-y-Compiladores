set -f
OUTPUT="$(cat | ../../build/Compiler)"
echo -e "Compiled the following query:\n"
echo -e "$OUTPUT"
echo -e "\n\nPostgres output:\n\n"
echo -e $OUTPUT | psql -h 172.25.16.1 -p 5432 -U postgres
