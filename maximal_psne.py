# load game configs
game_file = open("game_config.txt", "r")
N = int(game_file.readline().strip())
C = int(game_file.readline().strip())
H = int(game_file.readline().strip())
game_file.close()

# read psnes
psne_file = open("psne_output.txt", "r")
psnes = []
psne_count = int(psne_file.readline().strip())
for i in range(psne_count):
    psnes.append([int(ele) for ele in psne_file.readline().strip().split()])
psne_file.close()

# read utilities
utility_file = open("utilities.txt", "r")
utilities = []
strat_space = int(utility_file.readline().strip())
while True:
    line = utility_file.readline()
    if not line:
        break
    line = line.strip().split()
    utilities.append([float(ele) for ele in line[1:]])
utility_file.close()


# read strategies
strategy_file = open("strategies.txt", "r")
strategies = []
while True:
    line = strategy_file.readline()
    if not line:
        break
    line = line.strip().split("->")
    strategies.append(line[1])
strategy_file.close()


# find max psne
max_psne = []
max_util = [0]
for psne in psnes:
    idx = 0
    for i, strat in enumerate(reversed(psne)):
        idx += strat * (strat_space ** i)
    if sum(utilities[idx]) > sum(max_util):
        max_util = utilities[idx]
        max_psne = psne

print("==========================================================")
print("Maximal PSNE:")
for player, strat in enumerate(max_psne):
    print("Player {}: Strategy: {}".format(player + 1, strategies[strat]))
    print("           Utility: {}".format(max_util[player]))
    print("\n")
print("==========================================================")
