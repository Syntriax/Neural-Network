#include <iostream>
#include <time.h>

#define RandomRange 1
#define InitialSynapseValue 0.0
#define MutationRate 0.15
#define CrossOverRate 0.1
#define PopCrossOverRate 0.95

class Synapse;
class Neuron;
class Layer;
class Input;
class Output;
class NeuralNetwork;
class Generation;

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
            float GetWeight();
            float GetValue();
            float GetBias();
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

    float Synapse::GetWeight()
    {
        return weight;
    }
    
    float Synapse::GetValue()
    {
        return value;
    }

    float Synapse::GetBias()
    {
        return bias;
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
            friend void WriteToFile(NeuralNetwork *);
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
            friend void WriteToFile(NeuralNetwork *);
            void Reset();
            void CrossOverNetwork(NeuralNetwork *);
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            bool SetLayer(int);
            float GetOutput(int);
            float GetError(int, float);
            float GetScore(int);
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
        std::cout << count << "\n";
        FILE *file = fopen("Data/BestSynapses.txt", "w");
        for (i = 0; i < count; i++)
        {
            fprintf(file, "%f, %f, ", synapsePtr -> GetWeight(), synapsePtr -> GetBias());
            synapsePtr++;
        }
        
        for (j = 0; j < network -> hiddenSize; j++)
        {
            count = (network -> hidden + j) -> synapseSize;
            std::cout << count << "\n";
            synapsePtr = (network -> hidden + j) -> synapses;
            for (i = 0; i < count; i++)
            {
                fprintf(file, "%f, %f, ", synapsePtr -> GetWeight(), synapsePtr -> GetBias());
                synapsePtr++;
            }
        }
        

        synapsePtr = network -> output -> synapses;
        count = network -> output -> synapseSize;
        std::cout << count << "\n";
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

    float NeuralNetwork::GetError(int index = 0, float target = 0.0)
    {
        float result = GetOutput(index) - target;
        return result < 0.0 ? -result : result;
    }

    float NeuralNetwork::GetScore(int index = 0)
    {
        float result = GetOutput(index);
        return result;
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
            void SortByScoreArray(float *, int, int);
            void DisplayScores(int);
            void DisplayBest(int);
            void SetTarget(float);
            void SetInput(float, int);
            void NextGeneration();
            void WriteBestToFile();
            bool CreateNetworks(int, int);
            bool ConnectNetworks();
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            float GetBest(int);
            float GetError(int);
            int GetStep();
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
            std::cout << i << " -> " << (networks + i) -> GetError(index) << "\n";
    }

    void Generation::DisplayBest(int index = 0)
    {
        std::cout << "Target -> " << target << "\tBest -> " << networks -> GetScore(index) << "\n";
    }

    float Generation::GetBest(int index = 0)
    {
        return networks -> GetScore(index);
    }

    float Generation::GetError(int index = 0)
    {
        return (networks + index) -> GetError(0, target);
    }
    void Generation::SortByScore(int index = 0)
    {
        int i;
        int j;
        for (i = 0; i < size - 1; i++)
            for (j = i + 1; j < size; j++)
                if((networks + i) -> GetError(index, target) > (networks + j) -> GetError(index, target))
                    _SwapNetworks((networks + i), (networks + j));
    }
    void Generation::SortByScoreArray(float *array, int size, int index = 0)
    {
        int i;
        int j;
        float temp;
        
        for (i = 0; i < size - 1; i++)
            for (j = i + 1; j < size; j++)
                if(*(array + i) > *(array + j))
                {
                    temp = *(array + i);
                    *(array + i) = *(array + j);
                    *(array + j) = temp;
                    _SwapNetworks((networks + i), (networks + j));
                }
                // if((networks + i) -> GetError(index, target) > (networks + j) -> GetError(index, target))
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

        // for (; i < size; i+=2)
        for (i = 2; i < crossOverCount; i+=2)
        {
            first = (networks + i);
            second = (networks + i + 1);

            first -> Copy(*(networks + 0));
            second -> Copy(*(networks + 1));

            if(RandomFloat(0, 1) < 0.5)
                first -> CrossOverNetwork(second);
            else
            {
                first -> MutateNetwork();
                second -> MutateNetwork();
            }
        }
        // SortByScore();
        
        for (; i < size; i++)
            (networks + i) -> RandomizeValues();
        step++;
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



int main()
{
    FILE *inputFile;
    FILE *outputFile;
    int decision;
    float currentError;
    int trainCounter;

    int inputCounter;
    int floatCounter;
    int i;
    int j;
    int generationCounter;
    float trainInputs[30][5];
    float scores[50];
    float testInputs[120][5];
    Generation generation(50, 5);
    inputFile = fopen("Data/train.data", "r");
    for (inputCounter = 0; inputCounter < 30; inputCounter++)
        for (floatCounter = 0; floatCounter < 5; floatCounter++)
            fscanf(inputFile, "%f,", &trainInputs[inputCounter][floatCounter]);
    fclose(inputFile);
    inputFile = fopen("Data/test.data", "r");
    for (inputCounter = 0; inputCounter < 150; inputCounter++)
        for (floatCounter = 0; floatCounter < 5; floatCounter++)
            fscanf(inputFile, "%f,", &testInputs[inputCounter][floatCounter]);
    fclose(inputFile);

    std::cout << "Inputs Are Getting Set: ";
    std::cout << generation.SetInputNeurons(4) << "\n";
    std::cout << "Hiddens Are Getting Set: ";
    std::cout << generation.SetHiddenNeurons(0, 2) << "\n";
    std::cout << "Hiddens Are Getting Set: ";
    std::cout << generation.SetHiddenNeurons(1, 2) << "\n";
    std::cout << "Hiddens Are Getting Set: ";
    std::cout << generation.SetHiddenNeurons(2, 2) << "\n";
    std::cout << "Hiddens Are Getting Set: ";
    std::cout << generation.SetHiddenNeurons(3, 2) << "\n";
    std::cout << "Hiddens Are Getting Set: ";
    std::cout << generation.SetHiddenNeurons(4, 2) << "\n";
    std::cout << "Inputs Are Getting Set: ";
    std::cout << generation.SetOutputNeurons(1) << "\n";
    std::cout << "Networks Are Getting Connected: ";
    std::cout << generation.ConnectNetworks() << "\n";

    generation.Randomize();
    
    do
    {
        std::cout << "-2 - Test\n-3 - Best to File\nAny Number for train count\n-1 - Exit\nDecision: ";
        std::cin >> decision;
        
        switch (decision)
        {
            case -3:
                generation.WriteBestToFile();
                break;
            default:
                trainCounter = 0;
                for (int lk = 0; lk < decision; lk++)
                {
                    std::cout << trainCounter++ << "\n";
                    for (inputCounter = 0; inputCounter < 10; inputCounter++)
                    {
                        // for (generationCounter = 0; generationCounter < 25; generationCounter++)
                        // {
                            for (j = 0; j < 50; j++)
                                scores[j] = 0.0;
                            for (i = 0; i < 3; i++)
                            {
                                for (floatCounter = 0; floatCounter < 4; floatCounter++)
                                    generation.SetInput(trainInputs[inputCounter * 3 + i][floatCounter], floatCounter);
                                generation.SetTarget(trainInputs[inputCounter * 3 + i][4]);
                                generation.Fire();
                                for (j = 0; j < 50; j++)
                                    scores[j] += generation.GetError(j);
                            }
                            generation.SortByScoreArray(scores, 50);
                            generation.NextGeneration();
                            // generation.NextGeneration();
                        // }
                    }
                }
                
                std::cout << "Best -> " << scores[0] << "\n";
                std::cout << "Train is Over!\n";
            case -2:
                outputFile = fopen("Data/results.data", "w");
                trainCounter = 0;
                for (inputCounter = 0; inputCounter < 120; inputCounter++)
                {
                    for (floatCounter = 0; floatCounter < 4; floatCounter++)
                        generation.SetInput(testInputs[inputCounter][floatCounter], floatCounter);
                    generation.SetTarget(testInputs[inputCounter][4]);
                    
                    generation.Fire();
                    // generation.DisplayBest();
                    currentError = testInputs[inputCounter][4] - generation.GetBest() < 0 ? generation.GetBest() - testInputs[inputCounter][4] : testInputs[inputCounter][4] - generation.GetBest();
                    // fprintf(outputFile, "Original = %f\t->\tTrained -> %f\t Error = %f\n", testInputs[inputCounter][4], generation.GetBest(), currentError);
                    fprintf(outputFile, "%f,%f,%f\n", testInputs[inputCounter][4], generation.GetBest(), currentError);
                }
                fclose(outputFile);
                std::cout << "Test is Over!\n";
                break;
        }
    } while (decision != -1);

    return 0;
}
