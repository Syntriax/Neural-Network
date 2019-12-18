#include <iostream>
#include <time.h>

#define RandomRange 1
#define InitialSynapseValue 0.0
#define MutationRate 0.0001

class Synapse;
class Neuron;
class Layer;
class Input;
class Output;
class NeuralNetwork;

float RandomFloat(int min, int max)
{
    float result;
    int value;
    static unsigned long int counter = time(0);
    srand(time(0) * counter++);
    value = ((rand() * counter) % ((max - min) * 100000));
    result = (float)value / 100000.0 + (float)min;
    // std::cout << "random is " << result << "\n";
    return result;
}

#pragma region Synapse
    class Synapse
    {
        private:
            float weight;
            float value;
            float bias;
        public:
            Synapse();
            void SetValue(float);
            void SetWeight(float);
            void SetBias(float);
            float Fire();
    };

    Synapse::Synapse()
    {
        this -> value = this -> weight = this -> bias = InitialSynapseValue;
    }

    void Synapse::SetValue(float value)
    {
        this -> value = value;
    }

    void Synapse::SetWeight(float weight)
    {
        this -> weight = weight;
    }

    void Synapse::SetBias(float bias)
    {
        this -> bias = bias;
    }

    float Synapse::Fire()
    {
        float result = 0.0;

        result = this -> value * this -> weight + this -> bias;

        return result;
    }
#pragma endregion
#pragma region Neuron
    class Neuron
    {
        private:
            Synapse *incomings;
            Synapse *forwards;
            int incomingsSize;
            int forwardsSize;
            int layerSize;
        public:
            Neuron();
            void ConnectIncomings(Synapse *, int);
            void ConnectForwards(Synapse *, int, int);
            void SetValue(float);
            void Reset();
            float GetValue();
    };

    Neuron::Neuron()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }

    void Neuron::Reset()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }

    void Neuron::SetValue(float value)
    {
        for (int i = 0; i < forwardsSize; i++)
            (forwards + i) -> SetValue(value);
    }

    void Neuron::ConnectIncomings(Synapse *incomings, int incomingsSize)
    {
        this -> incomings = incomings;
        this -> incomingsSize = incomingsSize;
    }

    void Neuron::ConnectForwards(Synapse *forwards, int forwardsSize, int layerSize)
    {
        this -> forwards = forwards;
        this -> forwardsSize = forwardsSize;
        this -> layerSize = layerSize;
    }

    float Neuron::GetValue()
    {
        float result = 0.0;

        if(!incomings) return result;

        for (int i = 0; i < incomingsSize; i++)
            result += (incomings + i) -> Fire();


        if(!forwards) return result;

        for (int i = 0; i < forwardsSize; i++)
            (forwards + i * layerSize) -> SetValue(result);

        return result;
    }
#pragma endregion
#pragma region Layer
    class Layer
    {
        protected:
            Neuron *neurons;
            Synapse *synapses;
            int neuronSize;
            int synapseSize;
            Neuron *_CreateNeurons(int);
            Synapse *_CreateSynapses(int);
        public:
            Layer();
            Layer(int);
            ~Layer();
            void FireLayer();
            void Mutate();
            void RandomizeValues();
            bool CreateNeurons(int);
            bool ConnectPrevious(Layer *);
            bool ConnectForwards(Layer *);
            int GetSize();
    };

    Layer::Layer()
    {
        neuronSize = synapseSize = 0;
        neurons = NULL;
        synapses = NULL;
    }

    Layer::Layer(int size)
    {
        neuronSize = synapseSize = 0;
        synapses = NULL;
        neurons = _CreateNeurons(size);
    }

    Layer::~Layer()
    {
        if(neurons)  delete neurons;
        if(synapses) delete synapses;
    }

    Neuron *Layer::_CreateNeurons(int size)
    {
        Neuron *newNeurons = NULL;
        newNeurons = new Neuron[size];

        if(newNeurons)
            for (int i = 0; i < size; i++)
                (newNeurons + i) -> Reset();

        return newNeurons;
    }

    Synapse *Layer::_CreateSynapses(int size)
    {
        Synapse *newSynapses = NULL;
        newSynapses = new Synapse[size];

        return newSynapses;
    }

    void Layer::FireLayer()
    {
        for (int i = 0; i < neuronSize; i++)
            (neurons + i) -> GetValue();
    }

    void Layer::RandomizeValues()
    {
        float bias;
        float weight;
        for (int i = 0; i < synapseSize; i++)
        {
            bias = RandomFloat(-RandomRange, RandomRange);
            weight = RandomFloat(-RandomRange, RandomRange);
            (synapses + i) -> SetBias(bias);
            (synapses + i) -> SetWeight(weight);
        }
    }

    void Layer::Mutate()
    {
        float bias = 0.0;
        float weight = 0.0;
        float mutationValue = 0.0;

        for (int i = 0; i < synapseSize; i++)
        {
            mutationValue = RandomFloat(0, 1);
            if(mutationValue <= MutationRate)
            {
                bias   = RandomFloat(-RandomRange, RandomRange);
                weight = RandomFloat(-RandomRange, RandomRange);
                (synapses + i) -> SetBias(bias);
                (synapses + i) -> SetWeight(weight);
            }
        }
    }

    bool Layer::CreateNeurons(int size)
    {
        if(neurons = _CreateNeurons(size))
            neuronSize = size;
        return neurons;
    }

    bool Layer::ConnectPrevious(Layer *previous)
    {
        int previousSize = previous -> GetSize();
        int synapseCount = (this -> neuronSize) * previousSize;
        int currentIndex = 0;
        Synapse *currentSynapse = NULL;
        Neuron *currentNeuron = NULL;

        if(synapses) delete synapses;
        // synapses = (Synapse *) new char[sizeof(Synapse) * synapseCount];
        synapses = _CreateSynapses(synapseCount);
        if(!synapses) return false;


        for (int thisNeuron = 0; thisNeuron < this -> neuronSize; thisNeuron++)
        {
            for (int prevNeuron = 0; prevNeuron < previousSize; prevNeuron++)
            {
                currentIndex = thisNeuron * previousSize + prevNeuron;
                currentSynapse = (synapses + currentIndex);
                currentNeuron = (previous -> neurons) + prevNeuron;
                // *currentSynapse = Synapse();
            }

            currentNeuron = (neurons + thisNeuron);
            currentNeuron -> ConnectIncomings((synapses + thisNeuron * previousSize), previousSize);
        }

        synapseSize = synapseCount;
        return previous -> ConnectForwards(this);
    }

    bool Layer::ConnectForwards(Layer *forwards)
    {
        int forwardsSize = forwards -> neuronSize;
        Neuron *currentNeuron = NULL;

        for (int thisNeuron = 0; thisNeuron < this -> neuronSize; thisNeuron++)
        {
            currentNeuron = (neurons + thisNeuron);
            for (int forwardNeuron = 0; forwardNeuron < forwardsSize; forwardNeuron++)
                currentNeuron -> ConnectForwards(forwards -> synapses + thisNeuron, forwardsSize, this -> neuronSize);
        }
        return true;
    }

    int Layer::GetSize()
    {
        return neuronSize;
    }
    #pragma region Input-Output
        class Input : public Layer
        {
            public:
                Input();
                void SetValue(float, int);
        };

        Input::Input() : Layer() {}
        void Input::SetValue(float value, int index = 0)
        {
            if(index >= this -> neuronSize || index < 0)
                return;

            (neurons + index) -> SetValue(value);
        }

        class Output : public Layer
        {
            public:
                Output();
                float GetValue(int);
        };

        Output::Output() : Layer() {}
        float Output::GetValue(int index = 0)
        {
            float result = 0.0;

            if(index >= this -> neuronSize || index < 0)
                return result;

            result = (neurons + index) -> GetValue();
            return result;
        }
    #pragma endregion
#pragma endregion
#pragma region NeuralNetwork
    class NeuralNetwork
    {
        private:
            Input *input;
            Layer *hidden;
            Output *output;
            int hiddenSize;
        public:
            NeuralNetwork();
            NeuralNetwork(int);
            ~NeuralNetwork();
            void FireNetwork();
            void RandomizeValues();
            void MutateNetwork();
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            bool SetLayer(int);
            float GetOutput(int);
            float GetScore(float, int);
            void SetInput(float, int);
    };

    NeuralNetwork::NeuralNetwork()
    {
        hiddenSize = 0;
        input = NULL;
        hidden = NULL;
        output = NULL;
    }

    NeuralNetwork::NeuralNetwork(int hiddenSize)
    {
        this -> hiddenSize = hiddenSize;
        input  = new Input();
        hidden = new Layer(hiddenSize);
        output = new Output();
    }

    NeuralNetwork::~NeuralNetwork()
    {
        if(input)  delete input;
        if(hidden) delete hidden;
        if(output) delete output;
    }

    void NeuralNetwork::FireNetwork()
    {
        for (int i = 0; i < hiddenSize; i++)
            (hidden + i) -> FireLayer();

        output -> FireLayer();
    }
    void NeuralNetwork::MutateNetwork()
    {
        input -> Mutate();

        for (int i = 0; i < hiddenSize; i++)
            (hidden + i) -> Mutate();

        output -> Mutate();
    }

    void NeuralNetwork::RandomizeValues()
    {
        input -> RandomizeValues();

        for (int i = 0; i < hiddenSize; i++)
            (hidden + i) -> RandomizeValues();

        output -> RandomizeValues();
    }

    bool NeuralNetwork::SetInputNeurons(int size)
    {
        return input -> CreateNeurons(size);
    }

    bool NeuralNetwork::SetHiddenNeurons(int index, int size)
    {
        return (hidden + index) -> CreateNeurons(size);
    }

    bool NeuralNetwork::SetOutputNeurons(int size)
    {
        return output -> CreateNeurons(size);
    }

    bool NeuralNetwork::ConnectLayers()
    {
        if(!hidden -> ConnectPrevious(input))
            return false;

        for (int i = 1; i < hiddenSize; i++)
            if(!(hidden + i) -> ConnectPrevious((hidden + i - 1)))
                return false;

        if(!output -> ConnectPrevious((hidden + hiddenSize - 1)))
            return false;

        return true;
    }

    bool NeuralNetwork::SetLayer(int hiddenSize)
    {
        this -> hiddenSize = hiddenSize;
        input  = new Input();
        hidden = new Layer(hiddenSize);
        output = new Output();
    }

    float NeuralNetwork::GetOutput(int index = 0)
    {
        return output -> GetValue(index);
    }

    float NeuralNetwork::GetScore(float target, int index = 0)
    {
        float result = GetOutput(index) - target;
        return result < 0.0 ? -result : result;
    }

    void NeuralNetwork::SetInput(float value, int index = 0)
    {
        input -> SetValue(value, index);
    }
#pragma endregion
#pragma region Generation
    class Generation
    {
        private:
            NeuralNetwork *networks;
            int size;
            int step;
            float target;
            void SwapNetworks(NeuralNetwork *, NeuralNetwork *);
            NeuralNetwork *_CreateNetworks(int, int);
        public:
            Generation();
            Generation(int, int);
            ~Generation();
            void Randomize();
            void Fire();
            void SortByScore(int);
            void DisplayScores(int);
            void SetTarget(float);
            void SetInput(float, int);
            bool CreateNetworks(int, int);
            bool ConnectNetworks();
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
    };

    Generation::Generation()
    {
        step = 0;
        networks = NULL;
        size = 0;
        target = 0.0;
    }

    Generation::Generation(int size, int hiddenSizes)
    {
        step = 0;
        target = 0.0;
        this -> size = size;
        networks = _CreateNetworks(size, hiddenSizes);
    }

    Generation::~Generation()
    {
        if(networks) delete networks;
    }

    NeuralNetwork *Generation::_CreateNetworks(int size, int hiddenSizes)
    {
        NeuralNetwork *newNetworks = NULL;
        newNetworks = new NeuralNetwork[size];

        if(newNetworks)
            for (int i = 0; i < size; i++)
                (newNetworks + i) -> SetLayer(hiddenSizes);

        return newNetworks;
    }

    void Generation::Randomize()
    {
        for (int i = 0; i < this -> size; i++)
            (networks + i) -> RandomizeValues();
    }

    void Generation::Fire()
    {
        for (int i = 0; i < this -> size; i++)
            (networks + i) -> FireNetwork();
    }

    void Generation::SwapNetworks(NeuralNetwork *first, NeuralNetwork *second)
    {
        NeuralNetwork temp;
        temp = *first;
        *first = *second;
        *second = temp;
    }

    void Generation::DisplayScores(int index = 0)
    {
        std::cout << "----Scores----\n";
        for (int i = 0; i < this -> size; i++)
            std::cout << i << " -> " << (networks + i) -> GetScore(target, index) << "\n";
    }

    void Generation::SortByScore(int index = 0)
    {
        for (int i = 0; i < size - 1; i++)
            for (int j = i + 1; j < size; j++)
                if((networks + i) -> GetScore(target, index) > (networks + j) -> GetScore(target, index))
                    SwapNetworks((networks + i), (networks + j));
    }

    void Generation::SetTarget(float target)
    {
        this -> target = target;
    }

    void Generation::SetInput(float value, int index = 0)
    {
        for (int i = 0; i < this -> size; i++)
            (networks + i) -> SetInput(value, index);
    }

    bool Generation::CreateNetworks(int size, int hiddenSizes)
    {
        if((networks = _CreateNetworks(size, hiddenSizes)))
            this -> size = size;
        return networks;
    }

    bool Generation::ConnectNetworks()
    {
        for (int i = 0; i < this -> size; i++)
            if(!(networks + i) -> ConnectLayers())
                return false;

        return true;
    }

    bool Generation::SetInputNeurons(int size)
    {
        for (int i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetInputNeurons(size))
                return false;
        return true;
    }

    bool Generation::SetHiddenNeurons(int index, int size)
    {
        for (int i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetHiddenNeurons(index, size))
                return false;
        return true;
    }

    bool Generation::SetOutputNeurons(int size)
    {
        for (int i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetOutputNeurons(size))
                return false;
        return true;
    }

#pragma endregion

int main(int argc, char const *argv[])
{
    Generation generation(50, 3);
    std::cout << "1 - ";
    std::cout << generation.SetInputNeurons(1) << "\n";
    std::cout << "2 - ";
    std::cout << generation.SetHiddenNeurons(0, 2) << "\n";
    std::cout << "3 - ";
    std::cout << generation.SetHiddenNeurons(1, 3) << "\n";
    std::cout << "4 - ";
    std::cout << generation.SetHiddenNeurons(2, 2) << "\n";
    std::cout << "5 - ";
    std::cout << generation.SetOutputNeurons(1) << "\n";
    std::cout << "6 - ";
    std::cout << generation.ConnectNetworks() << "\n";

    // generation.SetTarget(12.30);

    generation.Randomize();
    generation.Fire();
    generation.DisplayScores();
    std::cout << "-----------SORTING-----------\n";
    generation.SortByScore();
    generation.DisplayScores();

    return 0;
}
