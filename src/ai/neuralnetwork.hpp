#pragma once

#include "defines.hpp"
#include <vector>
#include <cmath>
#include <tuple>
#include <random>
#include "utils/rect.hpp"

namespace activationFunctions {

inline f64 binStep(f64 x) noexcept {
    if (x > 0.0)
        return 1.0;
    else
        return 0.0;
}

inline f64 sigimoid(f64 x) noexcept {
    return 1 / (1 + std::exp(-x));
}

} // namespace activationFunctions

class NeuralNetwork;

class Neuron {
    friend class NeuralNetwork;
    friend class Layer;

public:
    Neuron(u32 numInputs);
    Neuron(const std::vector<f64>& weights, f64 bias);
    Neuron(const Neuron& parent0, const Neuron& parent1, std::mt19937 rng);
    ~Neuron() = default;

    f64 activate(const std::vector<f64>& inputs) const;
    void mutate(f64 weightRange, f64 biasRange, std::mt19937 rng);

private:
    void draw(utils::Rect<i32> gameScreen, f64 x, f64 y) const;
    void drawText(utils::Rect<i32> gameScreen, f64 x, f64 y) const;

private:
    u32 m_numInputs = 0;
    std::vector<f64> m_weights;
    f64 m_bias = 0.0;
};

class Layer {
    friend class NeuralNetwork;

public:
    Layer(u32 numNeurons, u32 numInputsPerNeuron);
    Layer(const std::vector<Neuron>& neurons);
    Layer(const Layer& parent0, const Layer& parent1, std::mt19937 rng);
    ~Layer() = default;

    std::vector<f64> foward(const std::vector<f64>& inputs) const;
    void mutate(f64 weightRange, f64 biasRange, std::mt19937 rng);

    inline u32 getNumNeurons() const noexcept { return m_numNeurons; }

private:
    void drawConnections(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const;
    void drawNeurons(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const;
    void drawInputNeurons(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const;

private:
    u32 m_numNeurons = 0;
    std::vector<Neuron> m_neurons;
};

class NeuralNetwork {
public:
    NeuralNetwork() = default;
    NeuralNetwork(const NeuralNetwork& parent0, const NeuralNetwork& parent1, std::mt19937 rng); // crossover
    ~NeuralNetwork() = default;

    void addLayer(u32 numNeurons, u32 numInputsPerNeuron);
    void addLayer(u32 numNeurons);
    void addLayer(const std::vector<Neuron>& neurons);

    std::vector<f64> calc(const std::vector<f64>& inputs);
    void mutate(f64 weightMaxRange, f64 biasRange, std::mt19937 rng);

    void draw(utils::Rect<i32> gameScreen) const;

private:
    std::vector<Layer> m_layers;
};
