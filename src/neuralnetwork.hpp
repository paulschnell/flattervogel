#pragma once

#include "defines.hpp"
#include <vector>
#include <cmath>

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

class Neuron {
public:
    Neuron(u32 numInputs);
    Neuron(const std::vector<f64>& weights, f64 bias);
    ~Neuron() = default;

    f64 activate(const std::vector<f64>& inputs) const;
    // TODO: "learn"-func

private:
    u32 m_numInputs = 0;
    std::vector<f64> m_weights;
    f64 m_bias = 0.0;
};

class Layer {
public:
    Layer(u32 numNeurons, u32 numInputsPerNeuron);
    explicit Layer(const std::vector<Neuron>& neurons);
    ~Layer() = default;

    std::vector<f64> foward(const std::vector<f64>& inputs) const;
    // TODO: "learn"-func

    inline u32 getNumNeurons() const noexcept { return m_numNeurons; }

private:
    u32 m_numNeurons = 0;
    std::vector<Neuron> m_neurons;
};

class NeuralNetwork {
public:
    NeuralNetwork() = default;
    ~NeuralNetwork() = default;

    void addLayer(u32 numNeurons, u32 numInputsPerNeuron);
    void addLayer(u32 numNeurons);
    void addLayer(const std::vector<Neuron>& neurons);

    std::vector<f64> calc(const std::vector<f64>& inputs);

    inline usize getNumLayers() const noexcept { return m_layers.size(); }

private:
    std::vector<Layer> m_layers;
};
