#include <iostream>
#include <time.h>

#define RandomRange 1
#define InitialSynapseValue 0.0
#define MutationRate 0.0001
#define CrossOverRate 0.1

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
            ~Synapse();
            void SetValue(float);
            void SetWeight(float);
            void SetBias(float);
            float Fire();
    };

    Synapse::Synapse()
    {
        // std::cout << "Created Synapse\n";
        this -> value = this -> weight = this -> bias = InitialSynapseValue;
    }

    Synapse::~Synapse()
    {
        // std::cout << "Deleted Synapse\n";
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
            ~Neuron();
            void ConnectIncomings(Synapse *, int);
            void ConnectForwards(Synapse *, int, int);
            void SetValue(float);
            void Reset();
            float GetValue();
    };

    Neuron::Neuron()
    {
        // std::cout << "Created Neuron\n";
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }        


    Neuron::~Neuron()
    {
        // std::cout << "Deleted Neuron\n";
    }

    void Neuron::Reset()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }

    void Neuron::SetValue(float value)
    {
        int i;
        for (i = 0; i < forwardsSize; i++)
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
        int i;
        float result = 0.0;

        if(!incomings) return result;

        for (i = 0; i < incomingsSize; i++)
            result += (incomings + i) -> Fire();


        if(!forwards) return result;

        for (i = 0; i < forwardsSize; i++)
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
            void _SwapSynapses(Synapse *, Synapse *);
            Neuron *_CreateNeurons(int);
            Synapse *_CreateSynapses(int);
        public:
            Layer();
            Layer(int);
            ~Layer();
            void CopySynapses(Layer *);
            void FireLayer();
            void Mutate();
            void RandomizeValues();
            void CrossOverSynapses(Layer *);
            bool CreateNeurons(int);
            bool ConnectPrevious(Layer *);
            bool ConnectForwards(Layer *);
            int GetSize();
    };

    Layer::Layer()
    {
        // std::cout << "Created Layer\n";
        neuronSize = synapseSize = 0;
        neurons = NULL;
        synapses = NULL;
    }

    Layer::Layer(int size)
    {
        // std::cout << "Deleted Layer\n";
        neuronSize = synapseSize = 0;
        synapses = NULL;
        neurons = _CreateNeurons(size);
    }

    Layer::~Layer()
    {
        // std::cout << "Deleted Layer\n";
        if(neurons)  delete neurons;
        if(synapses) delete synapses;
    }
    
    void Layer::_SwapSynapses(Synapse *first, Synapse *second)
    {
        Synapse temporary = Synapse();
        temporary = *first;
        *first = *second;
        *second = temporary;
    }

    Neuron *Layer::_CreateNeurons(int size)
    {
        int i;
        Neuron *newNeurons = NULL;
        
        newNeurons = new Neuron[size];

        if(newNeurons)
            for (i = 0; i < size; i++)
                (newNeurons + i) -> Reset();

        return newNeurons;
    }

    Synapse *Layer::_CreateSynapses(int size)
    {
        Synapse *newSynapses = NULL;
        newSynapses = new Synapse[size];

        return newSynapses;
    }

    void Layer::CopySynapses(Layer *from)
    {
        int counter;
        for (counter = 0; counter < this -> synapseSize; counter++)
            *(synapses + counter) = *((from -> synapses) + counter);
    }

    void Layer::FireLayer()
    {
        int i;
        for (i = 0; i < neuronSize; i++)
            (neurons + i) -> GetValue();
    }

    void Layer::RandomizeValues()
    {
        float bias;
        float weight;
        int i;

        for (i = 0; i < synapseSize; i++)
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
        int i;

        for (i = 0; i < synapseSize; i++)
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

    void Layer::CrossOverSynapses(Layer *other)
    {
        int thisCounter;
        for (thisCounter = 0; thisCounter < synapseSize; thisCounter++)
            if(RandomFloat(0, 1) < CrossOverRate)
                _SwapSynapses((synapses + thisCounter), (other -> synapses + thisCounter));
    }

    bool Layer::CreateNeurons(int size)
    {
        if((neurons = _CreateNeurons(size)))
            neuronSize = size;
        return neurons;
    }

    bool Layer::ConnectPrevious(Layer *previous)
    {
        int previousSize = previous -> GetSize();
        int synapseCount = (this -> neuronSize) * previousSize;
        int thisNeuron;
        int prevNeuron;
        Neuron *currentNeuron = NULL;

        if(synapses)
        {
            delete synapses;
            synapses = NULL;
        }
        // synapses = (Synapse *) new char[sizeof(Synapse) * synapseCount];
        synapses = _CreateSynapses(synapseCount);
        if(!synapses) return false;


        for (thisNeuron = 0; thisNeuron < this -> neuronSize; thisNeuron++)
        {
            for (prevNeuron = 0; prevNeuron < previousSize; prevNeuron++)
                currentNeuron = (previous -> neurons) + prevNeuron;

            currentNeuron = (neurons + thisNeuron);
            currentNeuron -> ConnectIncomings((synapses + thisNeuron * previousSize), previousSize);
        }

        synapseSize = synapseCount;
        return previous -> ConnectForwards(this);
    }

    bool Layer::ConnectForwards(Layer *forwards)
    {
        int forwardsSize = forwards -> neuronSize;
        int thisNeuron;
        int forwardNeuron;
        Neuron *currentNeuron = NULL;

        for (thisNeuron = 0; thisNeuron < this -> neuronSize; thisNeuron++)
        {
            currentNeuron = (neurons + thisNeuron);
            for (forwardNeuron = 0; forwardNeuron < forwardsSize; forwardNeuron++)
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
            Input *_CreateInput();
            Layer *_CreateLayers(int);
            Output *_CreateOutput();
        public:
            NeuralNetwork();
            NeuralNetwork(int);
            ~NeuralNetwork();
            void Copy(const NeuralNetwork &);
            void FireNetwork();
            void RandomizeValues();
            void MutateNetwork();
            void Reset();
            void CrossOverNetwork(NeuralNetwork *);
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            bool SetLayer(int);
            float GetOutput(int);
            float GetScore(float, int);
            int GetHiddenSize();
            void SetInput(float, int);
    };

    Input *NeuralNetwork::_CreateInput()
    {
        Input *newInputs = NULL;
        newInputs = new Input();

        return newInputs;
    }

    Layer *NeuralNetwork::_CreateLayers(int size)
    {
        Layer *newLayers = NULL;
        newLayers = new Layer[size];

        return newLayers;
    }

    Output *NeuralNetwork::_CreateOutput()
    {
        Output *newOutputs = NULL;
        newOutputs = new Output();

        return newOutputs;
    }

    NeuralNetwork::NeuralNetwork()
    {
        // std::cout << "Created NeuralNetwork\n";
        hiddenSize = 0;
        input = NULL;
        hidden = NULL;
        output = NULL;
    }

    NeuralNetwork::NeuralNetwork(int hiddenSize)
    {
        // std::cout << "Created NeuralNetwork\n";
        this -> hiddenSize = hiddenSize;
        input  = _CreateInput();
        hidden = _CreateLayers(hiddenSize);
        output = _CreateOutput();
    }

    NeuralNetwork::~NeuralNetwork()
    {
        // std::cout << "Deleted NeuralNetwork\n";
        if(input)  
            delete input;
        if(hidden) 
            delete hidden;
        if(output)
            delete output;
    }

    void NeuralNetwork::Copy(const NeuralNetwork &parameter)
    {
        int i;

        input -> CopySynapses(parameter.input);
        
        for (i = 0; i < hiddenSize; i++)
            (hidden + i) -> CopySynapses(parameter.hidden + i);
            
        output -> CopySynapses(parameter.output);
    }

    void NeuralNetwork::FireNetwork()
    {
        int i;

        for (i = 0; i < hiddenSize; i++)
            (hidden + i) -> FireLayer();

        output -> FireLayer();
    }

    void NeuralNetwork::MutateNetwork()
    {
        int i;

        input -> Mutate();

        for (i = 0; i < hiddenSize; i++)
            (hidden + i) -> Mutate();

        output -> Mutate();
    }

    void NeuralNetwork::CrossOverNetwork(NeuralNetwork *other)
    {
        int i;

        input -> CrossOverSynapses(other -> input);

        for (i = 0; i < hiddenSize; i++)
            (hidden + i) -> CrossOverSynapses((other -> hidden) + i);

        output -> CrossOverSynapses(other -> output);
    }

    void NeuralNetwork::RandomizeValues()
    {
        int i;

        input -> RandomizeValues();

        for (i = 0; i < hiddenSize; i++)
            (hidden + i) -> RandomizeValues();

        output -> RandomizeValues();
    }

    void NeuralNetwork::Reset()
    {
        input = NULL;
        hidden = NULL;
        output = NULL;
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
        int i;

        if(!hidden -> ConnectPrevious(input))
            return false;

        for (i = 1; i < hiddenSize; i++)
            if(!(hidden + i) -> ConnectPrevious((hidden + i - 1)))
                return false;

        if(!output -> ConnectPrevious((hidden + hiddenSize - 1)))
            return false;

        return true;
    }

    bool NeuralNetwork::SetLayer(int hiddenSize)
    {
        this -> hiddenSize = hiddenSize;
        input  = _CreateInput();
        hidden = _CreateLayers(hiddenSize);
        output = _CreateOutput();
        return input && hidden && output;
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
    int NeuralNetwork::GetHiddenSize()
    {
        return hiddenSize;
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
            void _SwapNetworks(NeuralNetwork *, NeuralNetwork *);
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
        // std::cout << "Created Generation\n";
        step = 0;
        networks = NULL;
        size = 0;
        target = 0.0;
    }

    Generation::Generation(int size, int hiddenSizes)
    {
        // std::cout << "Created Generation\n";
        step = 0;
        target = 0.0;
        this -> size = size;
        networks = _CreateNetworks(size, hiddenSizes);
    }

    Generation::~Generation()
    {
        // std::cout << "Deleted Generation\n";
        if(networks) delete networks;
    }

    NeuralNetwork *Generation::_CreateNetworks(int size, int hiddenSizes)
    {
        int i;
        NeuralNetwork *newNetworks = NULL;
        newNetworks = new NeuralNetwork[size];

        if(newNetworks)
            for (i = 0; i < size; i++)
                (newNetworks + i) -> SetLayer(hiddenSizes);

        return newNetworks;
    }

    void Generation::Randomize()
    {
        int i;
        for (i = 0; i < this -> size; i++)
            (networks + i) -> RandomizeValues();
    }

    void Generation::Fire()
    {
        int i;
        for (i = 0; i < this -> size; i++)
            (networks + i) -> FireNetwork();
    }

    void Generation::_SwapNetworks(NeuralNetwork *first, NeuralNetwork *second)
    {
        NeuralNetwork temp;
        temp = *first;
        *first = *second;
        *second = temp;
        temp.Reset();
    }

    void Generation::DisplayScores(int index = 0)
    {
        int i;
        std::cout << "----Scores----\n";
        for (i = 0; i < this -> size; i++)
            std::cout << i << " -> " << (networks + i) -> GetScore(target, index) << "\n";
    }

    void Generation::SortByScore(int index = 0)
    {
        int i;
        int j;
        for (i = 0; i < size - 1; i++)
            for (j = i + 1; j < size; j++)
                if((networks + i) -> GetScore(target, index) > (networks + j) -> GetScore(target, index))
                    _SwapNetworks((networks + i), (networks + j));
    }

    void Generation::SetTarget(float target)
    {
        this -> target = target;
    }

    void Generation::SetInput(float value, int index = 0)
    {
        int i;
        for (i = 0; i < this -> size; i++)
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
        int i;
        for (i = 0; i < this -> size; i++)
            if(!(networks + i) -> ConnectLayers())
                return false;

        return true;
    }

    bool Generation::SetInputNeurons(int size)
    {
        int i;
        for (i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetInputNeurons(size))
                return false;
        return true;
    }

    bool Generation::SetHiddenNeurons(int index, int size)
    {
        int i;
        for (i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetHiddenNeurons(index, size))
                return false;
        return true;
    }

    bool Generation::SetOutputNeurons(int size)
    {
        int i;
        for (i = 0; i < this -> size; i++)
            if(!(networks + i) -> SetOutputNeurons(size))
                return false;
        return true;
    }

#pragma endregion
int main()
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
