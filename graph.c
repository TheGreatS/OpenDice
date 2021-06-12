#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "graph.h"

void print_graph_array(Graph **graph_array, int size){
    for (int i = 0; i < size; i++) {
        printf("|%p", graph_array[i]);
    }
    printf("\n");
}

Graph evaluate_equation_graph(Equation *equation, struct arguments *arguments) {
    double *num_stack = malloc(sizeof(double*) * (equation->num_count + 1));
    int num_count = 0;
    int stack_top = -1;
    Graph result_graph;
    init_graph(&result_graph, 0);
    Graph *graph_array[equation->num_count + 1];
    for (int i = 0; i < equation->op_count; i++) {
        if (equation->operators[i] == '.') {
            num_stack[++stack_top] = equation->numbers[num_count++];
            graph_array[stack_top] = NULL;
        } else {
            double result = 0;
            print_graph_array(graph_array, stack_top+1);
            int on_graph = graph_array[stack_top] != NULL;
            int prev_graph = graph_array[stack_top - 1] != NULL;
            switch(equation->operators[i]) {
            case '+': { // addition
                if (on_graph || prev_graph) {
                    for (int j = 0; j < result_graph.used; j++) {
                        if (on_graph && !prev_graph) {
                            graph_array[stack_top]->graphLines[j].line += num_stack[stack_top - 1];
                        } else if (!on_graph && prev_graph) {
                            graph_array[stack_top - 1]->graphLines[j].line += num_stack[stack_top];
                        } else if (on_graph && prev_graph) {
                            // TODO add two graphs together
                        } else {
                            // How did you get here?
                        }
                    }
                    if (on_graph) {
                        graph_array[stack_top - 1] = graph_array[stack_top];
                        result_graph = *graph_array[stack_top - 1];
                        if (!prev_graph) {
                            graph_array[stack_top] = NULL;
                        } else {
                            free_graph(graph_array[stack_top]);
                        }
                    }
                    num_stack[--stack_top] = 0;
                } else {
                    result = num_stack[stack_top - 1] + num_stack[stack_top];
                    num_stack[--stack_top] = result;
                    graph_array[stack_top] = NULL;
                }
            } break;
            case 'd': { // roll dice
                free_graph(&result_graph);
                result_graph = graph('d', num_stack[stack_top - 1], num_stack[stack_top]);
                num_stack[--stack_top] = 0;
                graph_array[stack_top] = &result_graph;
            } break;
            default:
                break;
            }
        }
    }

    free(num_stack);
    return result_graph;
}

// TODO replace with PrimeSwing algorithm
double factorial(double n) {
    return n <= 1 ? 1 : n * factorial(n-1);
}

double choose(double n, double k) {
    if (k > n) {
        return 0;
    } else if (k == 0 || n == k) {
        return 1;
    }
    return factorial(n) / (factorial(k) * factorial(n - k));
}

double calculate_probability(double value, double left, double right) {
    double total = 0;
    int max = floor((value-left)/right);
    for (int i = 0; i <= max; i++) {
        // neg is 1 if i is even, and -1 if i is odd
        double neg = (double) 1 + (-2 * (i & 1));
        double choose1 = choose(left, i);
        double choose2 = choose(value - (right * i) - 1, left - 1);
        total += neg * choose1 * choose2;
    }
    return (1 / pow(right, left)) * total;
}

Graph graph(char op, double left, double right) {
    Graph graph;
    init_graph(&graph, (int) left * right);
    double integral_left;
    double fractional_left = modf(left, &integral_left);
    double integral_right;
    modf(right, &integral_right);
    if (fractional_left == 0) {
        int min = (int) integral_left;
        int max = (int) integral_left * integral_right;
        for (int i = min; i <= max; i++) {
            GraphLine line;
            line.line = i;
            line.probability = calculate_probability(i, left, right);
            insert_into_graph(&graph, &line);

            // set max
            if (line.probability > graph.max)
                graph.max = line.probability;
            // set min
            if (line.probability < graph.min)
                graph.min = line.probability;
        }
    } else {
        //TODO generate fractional graph
    }
    return graph;
}

Graph* combine_graphs(char op, Graph* left, Graph* right) {
    Graph* graph = NULL;
    switch(op) {
        case '+':{
        } break;
        case '-':{
        }break;
        case '*':{
        }break;
        case '/':{
        }break;
        case '^':{
        }break;
        case 'd':{
        }break;
        case 'c':{
        }break;
        default:
            break;
    }
    free_graph(left);
    free_graph(right);
    return graph;
}

int find_graph_line(Graph* graph, int l, int r, double line) {
    if (r >= l) {
        int mid = l + (r - 1) / 2;
        if (graph->graphLines[mid].line == line)
            return mid;

        if (graph->graphLines[mid].line > line)
            return find_graph_line(graph, l, mid - 1, line);

        return find_graph_line(graph, mid + 1, r, line);
    }
    return -1;
}

// dynamic array functions
void init_graph(Graph* in, size_t initSize) {
    in->graphLines = malloc(initSize * sizeof(GraphLine));
    in->used = 0;
    in->size = initSize;
    in->min = 1;
    in->max = 0;
}
void insert_into_graph(Graph* in, GraphLine* element) {
    if (in->used == in->size) {
        in->size *= 2;
        in->graphLines = realloc(in->graphLines, in->size * sizeof(GraphLine));
    }
    in->graphLines[in->used].line = element->line;
    in->graphLines[in->used].probability = element->probability;
    in->used++;
}
void free_graph(Graph* in) {
    free(in->graphLines);
    in->graphLines = NULL;
    in->used = in->size = 0;
}
