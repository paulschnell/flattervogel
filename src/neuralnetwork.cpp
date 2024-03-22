#include "neuralnetwork.hpp"

#include <random>
#include <iostream>

Neuron::Neuron(u32 numInputs)
    : m_numInputs(numInputs) {
    std::random_device dev;
    std::mt19937 rng(dev());

    for (u32 i = 0; i < m_numInputs; i++) {
        std::uniform_real_distribution<f64> dist(0.0, 1.0);
        m_weights.push_back(dist(rng));
    }

    std::uniform_real_distribution<f64> dist(0.0, 1.0);
    m_bias = dist(rng);
}

Neuron::Neuron(const std::vector<f64>& weights, f64 bias)
    : m_numInputs(weights.size()), m_bias(bias) {
    m_weights.assign(weights.begin(), weights.end());
}

f64 Neuron::activate(const std::vector<f64>& inputs) const {
    f64 sum = m_bias;
    for (u32 i = 0; i < m_numInputs; i++) {
        sum += inputs[i] * m_weights[i];
    }

    return activationFunctions::sigimoid(sum);
}

Layer::Layer(u32 numNeurons, u32 numInputsPerNeuron)
    : m_numNeurons(numNeurons) {
    for (u32 i = 0; i < m_numNeurons; i++) {
        m_neurons.emplace_back(numInputsPerNeuron);
    }
}

Layer::Layer(const std::vector<Neuron>& neurons)
    : m_numNeurons(neurons.size()) {
    m_neurons.assign(neurons.begin(), neurons.end());
}

std::vector<f64> Layer::foward(const std::vector<f64>& inputs) const {
    std::vector<f64> output;
    output.reserve(m_numNeurons);
    for (const auto& ele : m_neurons) {
        output.push_back(ele.activate(inputs));
    }
    return output;
}

void NeuralNetwork::addLayer(u32 numNeurons, u32 numInputsPerNeuron) {
    m_layers.emplace_back(numNeurons, numInputsPerNeuron);
}

void NeuralNetwork::addLayer(u32 numNeurons) {
    m_layers.emplace_back(numNeurons, m_layers.back().getNumNeurons());
}

void NeuralNetwork::addLayer(const std::vector<Neuron>& neurons) {
    m_layers.emplace_back(neurons);
}

std::vector<f64> NeuralNetwork::calc(const std::vector<f64>& inputs) {
    std::vector<f64> temp(inputs);
    for (const auto& layer : m_layers) {
        temp = layer.foward(temp);
    }
    return temp;
}
