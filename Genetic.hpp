#include <iostream>
#include <time.h>

#define RandomRange 1
#define InitialSynapseValue 0.0
#define MutationRate 0.15
#define CrossOverRate 0.25
#define PopCrossOverRate 0.5

class Synapse;
class Neuron;
class Layer;
class Input;
class Output;
class NeuralNetwork;
class Generation;

double RandomDouble(int min, int max)
{
    double result;
    long int value;
    static unsigned long int counter = time(0);
    srand(time(0) * counter++);
    value = ((rand() * counter) % ((max - min) * 100000000));
    result = (double)value / 100000000.0 + (double)min;
    return result;
}

#pragma region Synapse
    class Synapse
    {
        private:
            double weight;
            double value;
            double bias;
        public:
            Synapse();
            ~Synapse();
            void SetValue(double);
            void SetWeight(double);
            void SetBias(double);
            double GetWeight();
            double GetValue();
            double GetBias();
            double Fire();
    };

    Synapse::Synapse()
    {
        this -> value = this -> weight = this -> bias = InitialSynapseValue;
    }

    Synapse::~Synapse()
    {
    }

    void Synapse::SetValue(double value)
    {
        this -> value = value;
    }

    void Synapse::SetWeight(double weight)
    {
        this -> weight = weight;
    }

    void Synapse::SetBias(double bias)
    {
        this -> bias = bias;
    }

    double Synapse::GetWeight()
    {
        return weight;
    }
    
    double Synapse::GetValue()
    {
        return value;
    }

    double Synapse::GetBias()
    {
        return bias;
    }

    double Synapse::Fire()
    {
        double result = 0.0;

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
            void SetValue(double);
            void Reset();
            double GetValue();
    };

    Neuron::Neuron()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }        


    Neuron::~Neuron()
    {
    }

    void Neuron::Reset()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
    }

    void Neuron::SetValue(double value)
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

    double Neuron::GetValue()
    {
        int i;
        double result = 0.0;

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
            friend void LoadFromFile(NeuralNetwork *, char *);
            friend void WriteToFile(NeuralNetwork *);
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
        double bias;
        double weight;
        int i;

        for (i = 0; i < synapseSize; i++)
        {
            bias = RandomDouble(-RandomRange, RandomRange);
            weight = RandomDouble(-RandomRange, RandomRange);
            (synapses + i) -> SetBias(bias);
            (synapses + i) -> SetWeight(weight);
        }
    }

    void Layer::Mutate()
    {
        double bias = 0.0;
        double weight = 0.0;
        double mutationValue = 0.0;
        bool isMutated = false;
        int i;

        for (i = 0; i < synapseSize; i++)
        {
            mutationValue = RandomDouble(0, 1);
            if(mutationValue <= MutationRate)
            {
                isMutated = true;
                bias   = RandomDouble(-RandomRange, RandomRange);
                weight = RandomDouble(-RandomRange, RandomRange);
                (synapses + i) -> SetBias(bias);
                (synapses + i) -> SetWeight(weight);
            }
        }

        if(!isMutated && synapseSize != 0)
            Mutate();
    }

    void Layer::CrossOverSynapses(Layer *other)
    {
        int thisCounter;
        for (thisCounter = 0; thisCounter < synapseSize; thisCounter++)
            if(RandomDouble(0, 1) < CrossOverRate)
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
                void SetValue(double, int);
        };

        Input::Input() : Layer() {}
        void Input::SetValue(double value, int index = 0)
        {
            if(index >= this -> neuronSize || index < 0)
                return;

            (neurons + index) -> SetValue(value);
        }

        class Output : public Layer
        {
            public:
                Output();
                double GetValue(int);
        };

        Output::Output() : Layer() {}
        double Output::GetValue(int index = 0)
        {
            double result = 0.0;

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
            double score;
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
            friend void LoadFromFile(NeuralNetwork *, char *);
            friend void WriteToFile(NeuralNetwork *);
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            bool SetLayer(int);
            double GetOutput(int);
            double GetError(int, double);
            double GetPrediction(int);
            double GetScore();
            int GetHiddenSize();
            void SetScore(double);
            void SetInput(double, int);
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
        hiddenSize = 0;
        input = NULL;
        hidden = NULL;
        output = NULL;
    }

    NeuralNetwork::NeuralNetwork(int hiddenSize)
    {
        this -> hiddenSize = hiddenSize;
        input  = _CreateInput();
        hidden = _CreateLayers(hiddenSize);
        output = _CreateOutput();
    }

    NeuralNetwork::~NeuralNetwork()
    {
        if(input)   delete input;
        if(hidden)  delete hidden;
        if(output)  delete output;
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

    void WriteToFile(NeuralNetwork *network)
    {
        int i;
        int j;
        Synapse *synapsePtr = network -> input -> synapses;
        int count = network -> input -> synapseSize;
        FILE *file = fopen("Data/BestSynapses.txt", "w");
        for (i = 0; i < count; i++)
        {
            fprintf(file, "%f, %f, ", synapsePtr -> GetWeight(), synapsePtr -> GetBias());
            synapsePtr++;
        }
        
        for (j = 0; j < network -> hiddenSize; j++)
        {
            count = (network -> hidden + j) -> synapseSize;
            synapsePtr = (network -> hidden + j) -> synapses;
            for (i = 0; i < count; i++)
            {
                fprintf(file, "%f, %f, ", synapsePtr -> GetWeight(), synapsePtr -> GetBias());
                synapsePtr++;
            }
        }
        

        synapsePtr = network -> output -> synapses;
        count = network -> output -> synapseSize;
        for (i = 0; i < count; i++)
        {
            fprintf(file, "%f, %f, ", synapsePtr -> GetWeight(), synapsePtr -> GetBias());
            synapsePtr++;
        }
        fclose(file);
    }

    void NeuralNetwork::Reset()
    {
        input = NULL;
        hidden = NULL;
        output = NULL;
    }

    void LoadFromFile(NeuralNetwork *network, char *filePath)
    {
        int i;
        int j;
        float readWeight;
        float readBias;
        Synapse *synapsePtr = network -> input -> synapses;
        int count = network -> input -> synapseSize;
        FILE *file = fopen(filePath, "r");
        for (i = 0; i < count; i++)
        {
            fscanf(file, "%f, %f, ", &readWeight, &readBias);
            synapsePtr -> SetWeight(readWeight);
            synapsePtr -> SetBias(readBias);
            synapsePtr++;
        }
        
        for (j = 0; j < network -> hiddenSize; j++)
        {
            count = (network -> hidden + j) -> synapseSize;
            synapsePtr = (network -> hidden + j) -> synapses;
            for (i = 0; i < count; i++)
            {
                fscanf(file, "%f, %f, ", &readWeight, &readBias);
                synapsePtr -> SetWeight(readWeight);
                synapsePtr -> SetBias(readBias);
                synapsePtr++;
            }
        }
        

        synapsePtr = network -> output -> synapses;
        count = network -> output -> synapseSize;
        for (i = 0; i < count; i++)
        {
            fscanf(file, "%f, %f, ", &readWeight, &readBias);
            synapsePtr -> SetWeight(readWeight);
            synapsePtr -> SetBias(readBias);
            synapsePtr++;
        }
        fclose(file);
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

    double NeuralNetwork::GetOutput(int index = 0)
    {
        return output -> GetValue(index);
    }

    double NeuralNetwork::GetError(int index = 0, double target = 0.0)
    {
        double result = GetOutput(index) - target;
        return result < 0.0 ? -result : result;
    }

    double NeuralNetwork::GetPrediction(int index = 0)
    {
        double result = GetOutput(index);
        return result;
    }

    double NeuralNetwork::GetScore()
    {
        return score;
    }

    int NeuralNetwork::GetHiddenSize()
    {
        return hiddenSize;
    }

    void NeuralNetwork::SetInput(double value, int index = 0)
    {
        input -> SetValue(value, index);
    }

    void NeuralNetwork::SetScore(double value)
    {
        score = value;
    }
#pragma endregion
#pragma region Generation
    class Generation
    {
        private:
            NeuralNetwork *networks;
            int size;
            int step;
            double target;
            void _SwapNetworks(NeuralNetwork *, NeuralNetwork *);
            NeuralNetwork *_CreateNetworks(int, int);
        public:
            Generation();
            Generation(int, int);
            ~Generation();
            void Randomize();
            void Fire();
            void SortByScore();
            void DisplayScores(int);
            void DisplayBest(int);
            void SetTarget(double);
            void SetInput(double, int);
            void NextGeneration();
            void WriteBestToFile();
            void UpdateScores(int);
            void ResetScores();
            void LoadBestFromFile(char *);
            bool CreateNetworks(int, int);
            bool ConnectNetworks();
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            double GetPredictionOfBestNetwork(int);
            double GetError(int);
            int GetStep();
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
            std::cout << i << " -> " << (networks + i) -> GetError(index) << "\n";
    }

    void Generation::DisplayBest(int index = 0)
    {
        std::cout << "Target -> " << target << "\tBest -> " << networks -> GetPrediction(index) << "\n";
    }

    void Generation::UpdateScores(int index = 0)
    {
        double scoreToAdd;
        int i;
        for (i = 0; i < size; i++)
        {
            scoreToAdd = (networks + i) -> GetError(index, target);
            (networks + i) -> SetScore((networks + i) -> GetScore() + scoreToAdd);
        }
    }

    void Generation::ResetScores()
    {
        int i;
        for (i = 0; i < size; i++)
            (networks + i) -> SetScore(0.0);
    }

    double Generation::GetPredictionOfBestNetwork(int index = 0)
    {
        return networks -> GetPrediction(index);
    }

    double Generation::GetError(int index = 0)
    {
        return (networks + index) -> GetError(0, target);
    }

    void Generation::SortByScore()
    {
        int i;
        int j;
        for (i = 0; i < size - 1; i++)
            for (j = i + 1; j < size; j++)
                if((networks + i) -> GetScore() > (networks + j) -> GetScore())
                    _SwapNetworks((networks + i), (networks + j));
    }

    void Generation::SetTarget(double target)
    {
        this -> target = target;
    }

    void Generation::SetInput(double value, int index = 0)
    {
        int i;
        for (i = 0; i < this -> size; i++)
            (networks + i) -> SetInput(value, index);
    }

    void Generation::WriteBestToFile()
    {
        WriteToFile(networks);
    }

    void Generation::NextGeneration()
    {
        int i = 2;
        int crossOverCount = size * PopCrossOverRate;

        if(i + crossOverCount >= size)
            return;

        NeuralNetwork *first = NULL;
        NeuralNetwork *second = NULL;
        Fire();

        for (i = 2; i < crossOverCount; i+=2)
        {
            first = (networks + i);
            second = (networks + i + 1);

            first -> Copy(*(networks + 0));
            second -> Copy(*(networks + 1));

            if(RandomDouble(0, 1) < 0.5)
                first -> CrossOverNetwork(second);
            else
            {
                first -> MutateNetwork();
                second -> MutateNetwork();
            }
        }
        
        for (; i < size; i++)
            (networks + i) -> RandomizeValues();
        step++;
    }

    void Generation::LoadBestFromFile(char *filePath)
    {
        LoadFromFile(networks, filePath);
        LoadFromFile(networks + 1, filePath);
        this -> NextGeneration();
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

    int Generation::GetStep()
    {
        return step;
    }
#pragma endregion
