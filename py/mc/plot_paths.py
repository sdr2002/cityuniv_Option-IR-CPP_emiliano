import numpy as np
import matplotlib.pyplot as plt

from option_pricer import GeometricBrownianMotion

def compare_simulated_paths_with_adjusted_layout(dynamics, simulations, random_seed, T, num_paths=100):
    """
    Compares simulated paths generated by simulate_paths and simulate_paths_dS in the same plot,
    with adjusted layout and width ratio for subplots.

    Parameters:
        dynamics (Dynamics): A Dynamics object implementing simulate_paths() and simulate_paths_dS().
        simulations (int): Number of simulations to generate.
        random_seed (int): Seed for reproducibility.
        T (float): Time to maturity.
        num_paths (int): Number of paths to visualize per group (default=100).
    """
    # Simulate paths for both methods
    paths_exact = dynamics.simulate_paths(simulations=simulations, random_seed=random_seed)
    paths_numerical = dynamics.simulate_paths_dS(simulations=simulations, random_seed=random_seed)

    time = np.linspace(0, T, paths_exact.shape[1])  # Time vector for x-axis

    # Compute mean and standard deviation for both methods
    mean_exact = np.mean(paths_exact, axis=0)
    std_exact = np.std(paths_exact, axis=0)
    mean_numerical = np.mean(paths_numerical, axis=0)
    std_numerical = np.std(paths_numerical, axis=0)
    
    # Extract the final prices
    final_prices_exact = paths_exact[:, -1, 0]
    final_prices_numerical = paths_numerical[:, -1, 0]
    
    # Create a dual plot with adjusted width ratio
    fig, (ax_paths, ax_dist) = plt.subplots(1, 2, gridspec_kw={'width_ratios': [5, 2]}, figsize=(16, 8))

    # Simulated paths subplot
    # Plot subset of paths for the exact GBM (simulate_paths)
    for i in range(min(num_paths, paths_exact.shape[0])):
        ax_paths.plot(time, paths_exact[i, :, 0], alpha=0.1, color='blue', label="GBM (simulate_paths)" if i == 0 else None)
    
    # Plot subset of paths for the numerical GBM (simulate_paths_dS)
    for i in range(min(num_paths, paths_numerical.shape[0])):
        ax_paths.plot(time, paths_numerical[i, :, 0], alpha=0.1, color='red', label="GBM (simulate_paths_dS)" if i == 0 else None)
    
    # Plot mean and variance bounds for the exact GBM
    ax_paths.plot(time, mean_exact[:, 0], color='blue', linewidth=2, linestyle='dashed', label='Mean (simulate_paths)')
    ax_paths.fill_between(time, mean_exact[:, 0] - std_exact[:, 0], mean_exact[:, 0] + std_exact[:, 0], color='blue', alpha=0.2, label='Variance Bounds (simulate_paths)')
    
    # Plot mean and variance bounds for the numerical GBM
    ax_paths.plot(time, mean_numerical[:, 0], color='red', linewidth=2, linestyle='dashed', label='Mean (simulate_paths_dS)')
    ax_paths.fill_between(time, mean_numerical[:, 0] - std_numerical[:, 0], mean_numerical[:, 0] + std_numerical[:, 0], color='red', alpha=0.2, label='Variance Bounds (simulate_paths_dS)')
    
    ax_paths.set_title("Comparison of Simulated Paths: GBM vs Numerical GBM", fontsize=16)
    ax_paths.set_xlabel("Time to Maturity", fontsize=14)
    ax_paths.set_ylabel("Simulated Asset Price", fontsize=14)
    ax_paths.legend(fontsize=12)
    ax_paths.grid(True, linestyle='--', alpha=0.7)
    
    # Distribution subplot
    ax_dist.hist(final_prices_exact, bins=30, density=True, color='blue', alpha=0.5, orientation='horizontal', label="Final Prices (simulate_paths)")
    ax_dist.hist(final_prices_numerical, bins=30, density=True, color='red', alpha=0.5, orientation='horizontal', label="Final Prices (simulate_paths_dS)")
    ax_dist.axhline(np.mean(final_prices_exact), color='blue', linestyle='dashed', linewidth=1.5, label='Mean (simulate_paths)')
    ax_dist.axhline(np.mean(final_prices_numerical), color='red', linestyle='dashed', linewidth=1.5, label='Mean (simulate_paths_dS)')
    
    ax_dist.set_xlabel("Density", fontsize=14)
    ax_dist.legend(fontsize=12)
    ax_dist.grid(True, linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    plt.show()

# Example usage
if __name__ == "__main__":
    # Parameters for the simulation
    P0 = [100, 120]  # Initial prices for two assets
    T = 1.0  # Time to maturity in years
    dt = 0.01  # Time step size (in years)
    vol = [0.2, 0.3]  # Annualized volatilities for each asset
    drift = [0.05, 0.04]  # Annualized drift rates for each asset
    correlation = [
        [1.0, 0.8],
        [0.8, 1.0]
    ]  # Correlation matrix between the two assets
    simulations = 1000
    random_seed = 42

    # Initialize dynamics object
    dynamics = GeometricBrownianMotion(P0, T, dt, vol, drift, correlation)

    # Plot paths for both simulate_paths and simulate_paths_dS in the same plot with adjusted layout
    compare_simulated_paths_with_adjusted_layout(dynamics, simulations, random_seed, T, num_paths=100)
