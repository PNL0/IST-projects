import pulp as pulp

def read_int_list():
    return list(map(int, input().split()))

num_possible_toys, num_special_packages, max_toys_per_day = read_int_list()

toy_profit = []
production_cap = []
variables = []

objective_function = 0
toys_per_day = 0

used_in_package = []
matrix = []

for _ in range(num_possible_toys):
    toy_profit_i, production_cap_i = read_int_list()
    toy_profit.append(toy_profit_i)
    colunas = []
    used_in_package.append(0)
    matrix.append(colunas)
    production_cap.append(production_cap_i)
    variables.append(pulp.LpVariable(f"x{len(variables)}", 0, production_cap_i, pulp.LpInteger))
    objective_function += toy_profit_i * variables[-1]
    toys_per_day += variables[-1]

packages = []

for _ in range(num_special_packages):
    package_list = read_int_list()
    packages.append(package_list)
    used_in_package[package_list[0] - 1] = 1
    used_in_package[package_list[1] - 1] = 1
    used_in_package[package_list[2] - 1] = 1
    min_cap_production = min(production_cap[i - 1] for i in package_list[:3])
    var = pulp.LpVariable(f"x{len(variables)}", 0, min_cap_production, pulp.LpInteger)
    variables.append(var)
    for i in package_list[:3]:
        matrix[i - 1].append(var)
    objective_function += package_list[3] * var
    toys_per_day += 3 * var

prob = pulp.LpProblem("Problem", pulp.LpMaximize)
prob += objective_function
prob += toys_per_day <= max_toys_per_day

for i in range(num_possible_toys):
    if used_in_package[i]:
        condition = sum(matrix[i]) + variables[i]
        prob += condition <= production_cap[i]
        print(condition,"<=", production_cap[i])

prob.solve(pulp.GLPK(msg=0))
print(int(prob.objective.value()))