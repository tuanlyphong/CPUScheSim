import re

def c_to_yuml(c_code):
    # Regular expression to parse C function declarations
    function_pattern = re.compile(r'(\w[\w\s\*]*)\s+(\w+)\(([^)]*)\);')

    yuml_functions = []

    for match in function_pattern.finditer(c_code):
        return_type, function_name, params = match.groups()

        # Process parameters
        param_list = params.split(',')
        yuml_params = []

        for param in param_list:
            # Simplify and format parameter declarations
            param = param.strip()
            if param:  # Skip empty parameters
                # Handling complex types like 'long int'
                param_parts = param.split()

                # Join multiple parts of the type, e.g., 'long int' should be 'long_int'
                param_type = '_'.join(param_parts[:-1]).replace('const ', 'const_')  # Handle const keyword
                param_name = param_parts[-1] if len(param_parts) > 1 else param_type
                yuml_params.append(f"{param_name}: {param_type}")

        # Create yUML format string
        yuml_function = f"+{function_name}({', '.join(yuml_params)})"
        yuml_functions.append(yuml_function)

    return ';'.join(yuml_functions)

# Example C function declarations
c_code = """
void SCHED_MODEL_Init(scheduler_model_t* model);
void SCHED_MODEL_Simulate(scheduler_model_t* model);

void SCHED_MODEL_SetAlgorithm(scheduler_model_t* model, scheduler_algorithm_e sched_algo);
void SCHED_MODEL_AddProcess(scheduler_model_t* model, const process_init_t* process);
void SCHED_MODEL_DeleteProcess(scheduler_model_t* model, int pid);

void SCHED_MODEL_RegisterProcessObserver(scheduler_model_t* model, const process_observer_t* proc_observer);
void SCHED_MODEL_RegisterSchedulerObserver(scheduler_model_t* model, const scheduler_observer_t* sched_observer);
void SCHED_MODEL_RegisterQueueObserver(scheduler_model_t* model, const queue_observer_t* queue_observer);
"""

# Convert to yUML
yuml_output = c_to_yuml(c_code)
print(yuml_output)
