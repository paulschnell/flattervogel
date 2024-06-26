#include "neuralnetwork.hpp"

#include "raylib.h"
#include "nlohmann/json.hpp"
#include <random>
#include <format>
#include <fstream>
#include <exception>
#include <iostream>

Neuron::Neuron(u32 numInputs)
    : m_numInputs(numInputs) {
    std::random_device dev;
    std::mt19937 rng(dev());

    for (u32 i = 0; i < m_numInputs; i++) {
        std::uniform_real_distribution<f64> dist(-1.0, 1.0);
        m_weights.push_back(dist(rng));
    }

    std::uniform_real_distribution<f64> dist(-1.0, 1.0);
    m_bias = dist(rng);
}

Neuron::Neuron(const std::vector<f64>& weights, f64 bias)
    : m_numInputs(weights.size()), m_bias(bias) {
    m_weights.assign(weights.begin(), weights.end());
}

Neuron::Neuron(const Neuron& parent0, const Neuron& parent1, std::mt19937 rng)
    : m_numInputs(parent0.m_numInputs) {
    std::uniform_real_distribution<f64> dist0(0.0, 1.0);

    m_bias = (dist0(rng) > 0.5 ? parent0 : parent1).m_bias;

    for (usize i = 0; i < m_numInputs; i++) {
        std::uniform_real_distribution<f64> dist1(0.0, 1.0);
        m_weights.emplace_back((dist1(rng) > 0.5 ? parent0 : parent1).m_weights[i]);
    }
}

f64 Neuron::activate(const std::vector<f64>& inputs) const {
    f64 sum = m_bias;
    for (u32 i = 0; i < m_numInputs; i++) {
        sum += inputs[i] * m_weights[i];
    }

    return activationFunctions::sigimoid(sum);
}
void Neuron::mutate(f64 weightMaxRange, f64 biasMaxRange, std::mt19937 rng) {
    std::uniform_real_distribution<f64> dist0(0.0, 1.0);

    if (dist0(rng) > 0.75) {
        std::uniform_real_distribution<f64> dist1(-biasMaxRange, biasMaxRange);
        m_bias += dist1(rng);
    }

    for (auto& weight : m_weights) {
        std::uniform_real_distribution<f64> dist1(0.0, 1.0);
        if (dist1(rng) > 0.75) {
            std::uniform_real_distribution<f64> dist2(-weightMaxRange, weightMaxRange);
            weight += dist2(rng);
        }
    }
}

void Neuron::draw(utils::Rect<i32> gameScreen, f64 x, f64 y) const {
    DrawCircle(
        gameScreen.left + gameScreen.right + x * gameScreen.right,
        gameScreen.top + y * gameScreen.bottom,
        0.025 * gameScreen.right,
        YELLOW);
    DrawCircleLines(
        gameScreen.left + gameScreen.right + x * gameScreen.right,
        gameScreen.top + y * gameScreen.bottom,
        0.025 * gameScreen.right,
        BLACK);
}

void Neuron::drawText(utils::Rect<i32> gameScreen, f64 x, f64 y) const {
    DrawText(
        std::format("{}", (i32) (m_bias * 100)).c_str(),
        gameScreen.left + gameScreen.right + (x - (m_bias > 0 ? 0.01 : 0.02)) * gameScreen.right,
        gameScreen.top + (y - 0.0075) * gameScreen.bottom,
        0.025 * gameScreen.right,
        BLACK);
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

Layer::Layer(const Layer& parent0, const Layer& parent1, std::mt19937 rng)
    : m_numNeurons(parent0.m_numNeurons) {
    for (usize i = 0; i < parent0.m_neurons.size(); i++) {
        m_neurons.emplace_back(parent0.m_neurons[i], parent1.m_neurons[i], rng);
    }
}

std::vector<f64> Layer::foward(const std::vector<f64>& inputs) const {
    std::vector<f64> output;
    output.reserve(m_numNeurons);
    for (const auto& ele : m_neurons) {
        output.push_back(ele.activate(inputs));
    }
    return output;
}

void Layer::mutate(f64 weightMaxRange, f64 biasMaxRange, std::mt19937 rng) {
    for (auto& neuron : m_neurons) {
        neuron.mutate(weightMaxRange, biasMaxRange, rng);
    }
}

void Layer::drawConnections(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const {
    f64 beforeX = x - 0.15;
    std::vector<f64> beforeY;
    beforeY.reserve(m_neurons[0].m_numInputs);
    for (i32 i = 0; i < m_neurons[0].m_numInputs; i++) {
        beforeY.push_back(middleY - (m_neurons[0].m_numInputs / 2.0 - i) * 0.07 + 0.035);
    }

    for (i32 i = 0; i < m_numNeurons; i++) {
        f64 y = middleY - (m_numNeurons / 2.0 - i) * 0.07 + 0.035;

        for (i32 j = 0; j < beforeY.size(); j++) {
            DrawLineEx(
                Vector2(gameScreen.left + gameScreen.right + x * gameScreen.right,
                    gameScreen.top + y * gameScreen.bottom),
                Vector2(gameScreen.left + gameScreen.right + beforeX * gameScreen.right,
                    gameScreen.top + beforeY[j] * gameScreen.bottom),
                std::abs(m_neurons[i].m_weights[j]) * gameScreen.right * 0.01,
                m_neurons[i].m_weights[j] > 0 ? RED : BLUE);
        }
    }
}

void Layer::drawNeurons(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const {
    for (i32 i = 0; i < m_numNeurons; i++) {
        m_neurons[i].draw(gameScreen, x, middleY - (m_numNeurons / 2.0 - i) * 0.07 + 0.035);
        m_neurons[i].drawText(gameScreen, x, middleY - (m_numNeurons / 2.0 - i) * 0.07 + 0.035);
    }
}

void Layer::drawInputNeurons(utils::Rect<i32> gameScreen, f64 middleY, f64 x) const {
    for (i32 i = 0; i < m_neurons[0].m_numInputs; i++)
        Neuron(0).draw(gameScreen, x, middleY - (m_neurons[0].m_numInputs / 2.0 - i) * 0.07 + 0.035);
}

NeuralNetwork::NeuralNetwork(std::string_view file) {
    std::ifstream ifs(file.data());
    if (ifs.bad()) {
        std::cout << "Could not find file (" << file.data() << ")." << std::endl;
        throw std::exception();
    }

    nlohmann::json obj;
    ifs >> obj;

    for (int i = 0; i < obj["data"].size(); i++) {
        std::vector<Neuron> neurons;
        for (int j = 0; j < obj["data"][i].size(); j++) {
            std::vector<f64> weights;
            for (int k = 0; k < obj["data"][i][j]["weights"].size(); k++) {
                weights.emplace_back(obj["data"][i][j]["weights"][k]);
            }
            neurons.emplace_back(weights, obj["data"][i][j]["bias"]);
        }
        m_layers.emplace_back(neurons);
    }
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork& parent0, const NeuralNetwork& parent1, std::mt19937 rng) {
    for (usize i = 0; i < parent0.m_layers.size(); i++) {
        m_layers.emplace_back(parent0.m_layers[i], parent1.m_layers[i], rng);
    }
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
void NeuralNetwork::mutate(f64 weightMaxRange, f64 biasMaxRange, std::mt19937 rng) {
    for (auto& layer : m_layers) {
        layer.mutate(weightMaxRange, biasMaxRange, rng);
    }
}

void NeuralNetwork::draw(utils::Rect<i32> gameScreen) const {
    // Calculate middleY
    u32 mostNodesInLayer = 0;
    for (const auto& layer : m_layers) {
        if (layer.m_neurons[0].m_numInputs > mostNodesInLayer)
            mostNodesInLayer = layer.m_neurons[0].m_numInputs;
    }
    f64 middleY = 1.0 - mostNodesInLayer * 0.04;

    {
        f64 currentX = 0.25;
        for (u32 i = 0; i < m_layers.size(); i++) {
            m_layers[i].drawConnections(gameScreen, middleY, currentX);
            currentX += 0.15;
        }
    }
    {
        f64 currentX = 0.1;
        m_layers[0].drawInputNeurons(gameScreen, middleY, currentX);
        currentX += 0.15;
        for (const auto& layer : m_layers) {
            layer.drawNeurons(gameScreen, middleY, currentX);
            currentX += 0.15;
        }
    }
}

void NeuralNetwork::serialize(std::string_view file) const {
    nlohmann::json obj;

    for (const auto& layer : m_layers) {
        nlohmann::json layerObj = nlohmann::json::array();

        for (const auto& neuron : layer.m_neurons) {
            nlohmann::json neuronObj = {
                { "bias", neuron.m_bias },
                { "weights", nlohmann::json::array() }
            };

            for (const auto& weight : neuron.m_weights) {
                neuronObj["weights"].push_back(weight);
            }

            layerObj.push_back(neuronObj);
        }

        obj["data"].push_back(layerObj);
    }

    std::ofstream ofs(file.data());
    ofs << obj.dump(3);
    ofs.close();
}
