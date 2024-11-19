import re

def parse_c_struct_to_yuml(c_code):
    # Regex patterns for parsing
    struct_name_pattern = re.compile(r"typedef\s+struct\s+{[^}]+}\s+(\w+);")
    member_pattern = re.compile(r"\s*([\w\s\*]+)\s+(\w+)(\[\w+\])?\s*;")  # Matches "type name[array_size];"
    function_pointer_pattern = re.compile(
        r"([\w\s\*]+)\s*\(\*\s*(\w+)\)\(([^)]*)\)\s*;"
    )  # Matches "return_type (*name)(parameters);"

    # Extract the struct name
    struct_name_match = struct_name_pattern.search(c_code)
    if not struct_name_match:
        raise ValueError("No valid struct definition found.")
    struct_name = struct_name_match.group(1)

    # Extract the body of the struct
    struct_body = c_code[c_code.find('{')+1:c_code.find('}')]
    members = struct_body.strip().split('\n')

    yuml_fields = []
    for member in members:
        member = member.strip()  # Remove leading/trailing whitespace
        # Skip empty lines and comments
        if not member or member.startswith("//"):
            continue

        # Check if it's a function pointer
        func_ptr_match = function_pointer_pattern.search(member)
        if func_ptr_match:
            return_type, func_name, parameters = func_ptr_match.groups()
            # Normalize return type
            normalized_return_type = "_".join(return_type.split())
            # Normalize parameter list
            parameters = parameters.strip()
            normalized_parameters = ", ".join(
                f"{p.split()[-1]}: {'_'.join(p.split()[:-1]).replace('*_', '*')}" for p in parameters.split(",")
            )
            yuml_fields.append(
                f"+\\<funcPtr\\> {func_name}({normalized_parameters}): {normalized_return_type}"
            )
            continue

        # Check if it's a regular member
        match = member_pattern.search(member)
        if match:
            raw_type, var_name, array_size = match.groups()
            # Normalize the data type by replacing spaces with underscores
            normalized_type = "_".join(raw_type.split())
            # Ensure pointer asterisk (*) remains at the end of the type
            if '*' in normalized_type:
                normalized_type = normalized_type.replace('_*', '*')  # Adjust pointer position
            if array_size:
                array_size = array_size.replace("[", r"［")  # Escape opening bracket
                array_size = array_size.replace("]", r"］")  # Escape closing bracket
                normalized_type += array_size  # Append array size to type
            yuml_fields.append(f"+{var_name}: {normalized_type}")

    yuml_representation = f"[{struct_name}|{';'.join(yuml_fields)}]"
    return yuml_representation

# Example C code
c_code = """
// Core scheduler model structure
typedef struct {
    scheduler_algorithm_e sched_algo;                              // Selected scheduling algorithm
    process_t             process_list[PROCESS_LIST_SIZE_MAX];     // List of processes
    int                   process_list_size;                       // Number of processes in the list
    process_observer_t    proc_observer[PROCESS_OBSERVER_SIZE];    // Observers for process events
    scheduler_observer_t  sched_observer[SCHEDULER_OBSERVER_SIZE]; // Observers for scheduler events
    queue_observer_t      queue_observer[QUEUE_OBSERVER_SIZE];     // Observers for queue events
} scheduler_model_t;
"""

# Parse and print yUML
yuml_output = parse_c_struct_to_yuml(c_code)
print(yuml_output)
