import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial.distance import cdist
import random

# Genetic algo stuff
def initial_population(pop_size, num_cities):
    population = []
    for _ in range(pop_size):
        individual = list(np.random.permutation(num_cities))
        population.append(individual)
    return population

def fitness(individual, dist_matrix):
    total_distance = 0
    for i in range(len(individual) - 1):
        total_distance += dist_matrix[individual[i], individual[i + 1]]
    total_distance += dist_matrix[individual[-1], individual[0]]  # Back to start point
    return total_distance

def selection(population, dist_matrix):
    fitness_scores = [fitness(individual, dist_matrix) for individual in population]
    selected_indices = np.argsort(fitness_scores)[:len(population) // 2]
    return [population[i] for i in selected_indices]

def crossover(parent1, parent2):
    size = len(parent1)
    child = [-1] * size 
    start, end = sorted(random.sample(range(size), 2))
    child[start:end+1] = parent1[start:end+1]
    
    current_pos = 0
    for i in range(size):
        if child[i] == -1: 
            while parent2[current_pos] in child:
                current_pos += 1 
            child[i] = parent2[current_pos]
    
    return child

def mutate(individual):
    size = len(individual)    
    i, j = random.sample(range(size), 2)
    individual[i], individual[j] = individual[j], individual[i]
    
    return individual

def generate_cities(num_cities):
    return np.random.rand(num_cities, 3) * 1000

def calculate_distances(cities):
    return cdist(cities, cities)

def genetic_algorithm(cities, population_size=100, generations=500):
    dist_matrix = calculate_distances(cities)
    population = initial_population(population_size, len(cities))
    
    for generation in range(generations):
        selected_population = selection(population, dist_matrix)
        next_population = selected_population[:]
        
        while len(next_population) < population_size:
            parent1, parent2 = random.sample(selected_population, 2)
            child = crossover(parent1, parent2)
            if random.random() < 0.5: 
                child = mutate(child)
            next_population.append(child)
        
        population = next_population
    
    best_individual = min(population, key=lambda x: fitness(x, dist_matrix))
    return best_individual

# Simulate drone pathing 
def drone_salesman(path, cities, radius=20):
    circling_path = []
    
    for i in range(len(path)):
        current_city = cities[path[i]]
        angle = 2 * np.pi * i / len(path)
        
        x_offset = radius * np.cos(angle)
        y_offset = radius * np.sin(angle)
        z_offset = radius * np.sin(angle)

        new_position = np.array([current_city[0] + x_offset, 
                                 current_city[1] + y_offset, 
                                 current_city[2] + z_offset])
        
        circling_path.append(new_position)
    
    return np.array(circling_path)

def plot_paths(cities, first_salesman_path, drone_path):
    # city_coords = cities[:, :2]
    
    # Plotting 3D paths
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot Troop path 
    path1_coords = cities[first_salesman_path, :2]  
    ax.plot(path1_coords[:, 0], path1_coords[:, 1], zs=np.zeros(len(path1_coords)), color='blue', label='Troop Path', marker='o')
    
    # Plot Drone path 
    path2_coords = drone_path
    ax.plot(path2_coords[:, 0], path2_coords[:, 1], path2_coords[:, 2], color='red', label='Drone Path', marker='x')
   
    ax.scatter(cities[:, 0], cities[:, 1], cities[:, 2], c='black', label='Coordinates')
    
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.legend()
    
    plt.show()

def main():
    num_cities = 35 
    cities = generate_cities(num_cities)  
    best_path = genetic_algorithm(cities, population_size=200, generations=500)
 
    drone_path = drone_salesman(best_path, cities, radius=5)

    plot_paths(cities, best_path, drone_path)

if __name__ == '__main__':
    main()
