#!/bin/bash

# Usage: ./cstruct_to_yuml.sh <file>
# <file> content example:
# // Core scheduler model structure
# typedef struct {
#     scheduler_algorithm_e sched_algo;                              // Selected scheduling algorithm
#     process_t             process_list[PROCESS_LIST_SIZE_MAX];     // List of processes
#     const int             process_list_size;                       // Number of processes in the list
#     process_observer_t    proc_observer[PROCESS_OBSERVER_SIZE];    // Observers for process events
#     scheduler_observer_t  sched_observer[SCHEDULER_OBSERVER_SIZE]; // Observers for scheduler events
#     queue_observer_t      queue_observer[QUEUE_OBSERVER_SIZE];     // Observers for queue events
# } scheduler_model_t;

# Input and output files
input_file="$1"
output_file="yuml.txt"

# Preprocess the input file to replace spaces in member names with underscores
sed -E 's/([[:space:]]+)([[:alpha:]]+)([[:space:]]+)([[:alpha:]]+)/\1\2_\4/g' "$input_file" > preprocessed.txt

# Initialize the output string
output=""

# Read the struct name from the typedef line
struct_name=$(grep -oP 'typedef struct \{[^}]*\}\s*\K\S+' "preprocessed.txt")

# If the struct name is empty, try another approach
if [ -z "$struct_name" ]; then
  struct_name=$(awk '/typedef struct/{p=1} /}/&&p{sub(/;/, ""); print $NF}' "preprocessed.txt")
fi

# Read and process each line of the preprocessed struct
while read -r line; do
  # Skip lines that don't contain member declarations
  if [[ ! "$line" == *";"* ]]; then
    continue
  fi

  # Extract member type and name
  member_type=$(echo "$line" | awk '{print $1}')
  member_name=$(echo "$line" | awk '{print $2}' | tr -d ';')

  # Append to the output string
  output+="+${member_name}: ${member_type};"
done < <(sed -n '/typedef struct {/,/}/p' "preprocessed.txt" | grep -v '{\|}')

# Format the final yUML string
yuml="[${struct_name}|${output%?}"

# Write to the output file
echo -n "$yuml" | sed 's/\]/\\\]/g' > "$output_file"
echo "]" >> "$output_file"

#echo "yUML syntax generated in $output_file"
cat "$output_file"

# Remove the temporary preprocessed file
rm preprocessed.txt
rm "$output_file"