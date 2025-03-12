import sys
import os
import glob
from subprocess import Popen, PIPE


def generate_args(binary, *params):
    arguments = [binary]
    arguments.extend(list(params))
    return arguments


def execute_binary(args):
    process = Popen(' '.join(args), shell=True, stdout=PIPE, stderr=PIPE)
    (std_output, std_error) = process.communicate()
    process.wait()
    rc = process.returncode

    return rc, std_output, std_error


def check_all_engine_correctness(binary_path, data_graph_path, query_folder_path, expected_results):
    # find all query graphs.
    query_graph_path_list = glob.glob('{0}/*.graph'.format(query_folder_path))

    for engine in ['LFTJ', 'EXPLORE']:
        for query_graph_path in query_graph_path_list:
            execution_args = generate_args(binary_path, '-d', data_graph_path, '-q', query_graph_path, '-filter', 'LDF', '-order', 'GQL', '-engine', engine)
            (rc, std_output, std_error) = execute_binary(execution_args)
            query_graph_name = os.path.splitext(os.path.basename(query_graph_path))[0]
            expected_embedding_num = expected_results[query_graph_name]
            if rc == 0:
                embedding_num = 0
                std_output_list = std_output.split(b'\n')
                for line in std_output_list:
                    if b'#Embeddings' in line:
                        embedding_num = int(line.split(b':')[1].strip())
                        break
                if embedding_num != expected_embedding_num:
                    print(f'{engine} engine {query_graph_name} is wrong. Expected {expected_embedding_num}, Output {embedding_num}')
                    exit(-1)
            else:
                print(f'{engine} engine {query_graph_name} error.')
                exit(-1)
        print(f'{engine} engine pass the correctness check.')


if __name__ == '__main__':
    exe_file = "../build/run"
    if not os.path.isfile(exe_file):
        print('The binary {0} does not exist.'.format(exe_file))
        exit(-1)

    # load expected results.
    input_expected_results = {}
    input_expected_results_file = 'expected_output.res'
    with open(input_expected_results_file, 'r') as f:
        for line in f:
                result_item = line.split(':')
                input_expected_results[result_item[0].strip()] = int(result_item[1].strip())

    input_data_graph_path = './data_graph/HPRD.graph'
    input_query_graph_folder_path = './query_graph/'

    check_all_engine_correctness(exe_file, input_data_graph_path, input_query_graph_folder_path, input_expected_results)

