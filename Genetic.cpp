#include <iostream>
#include <time.h>

#define RandomRange 1
#define InitialSynapseValue 0.0
#define MutationRate 0.25
#define CrossOverRate 0.25
#define PopCrossOverRate 0.7

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
        this -> value = this -> weight = this -> bias = InitialSynapseValue;
    }

    Synapse::~Synapse()
    {
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
            float score;
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
            friend void WriteToFile(NeuralNetwork *);
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            bool SetLayer(int);
            float GetOutput(int);
            float GetError(int, float);
            float GetPrediction(int);
            float GetScore();
            int GetHiddenSize();
            void SetScore(float);
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

    float NeuralNetwork::GetPrediction(int index = 0)
    {
        float result = GetOutput(index);
        return result;
    }

    float NeuralNetwork::GetScore()
    {
        return score;
    }

    int NeuralNetwork::GetHiddenSize()
    {
        return hiddenSize;
    }

    void NeuralNetwork::SetInput(float value, int index = 0)
    {
        input -> SetValue(value, index);
    }

    void NeuralNetwork::SetScore(float value)
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
            float target;
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
            void SetTarget(float);
            void SetInput(float, int);
            void NextGeneration();
            void WriteBestToFile();
            void UpdateScores();
            void ResetScores();
            bool CreateNetworks(int, int);
            bool ConnectNetworks();
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            float GetBestPrediction(int);
            float GetError(int);
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

    void Generation::UpdateScores()
    {
        float scoreToAdd;
        int i;
        for (i = 0; i < size; i++)
        {
            scoreToAdd = (networks + i) -> GetError(0, target);
            (networks + i) -> SetScore((networks + i) -> GetScore() + scoreToAdd);
        }
    }

    void Generation::ResetScores()
    {
        int i;
        for (i = 0; i < size; i++)
            (networks + i) -> SetScore(0.0);
    }

    float Generation::GetBestPrediction(int index = 0)
    {
        return networks -> GetPrediction(index);
    }

    float Generation::GetError(int index = 0)
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
    int groupSize;
    float trainInputs[30][5];
    float testInputs[120][5];
    Generation generation(50, 5);

    inputFile = fopen("Data/train.data", "r");
    for (inputCounter = 0; inputCounter < 30; inputCounter++)
        for (floatCounter = 0; floatCounter < 5; floatCounter++)
            fscanf(inputFile, "%f,", &trainInputs[inputCounter][floatCounter]);
    fclose(inputFile);

    inputFile = fopen("Data/test.data", "r");
    for (inputCounter = 0; inputCounter < 120; inputCounter++)
        for (floatCounter = 0; floatCounter < 5; floatCounter++)
            fscanf(inputFile, "%f,", &testInputs[inputCounter][floatCounter]);
    fclose(inputFile);

    std::cout << "Inputs   Are Getting Set: ";
    std::cout << (generation.SetInputNeurons(4) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Hidden 1 Are Getting Set: ";
    std::cout << (generation.SetHiddenNeurons(0, 2) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Hidden 2 Are Getting Set: ";
    std::cout << (generation.SetHiddenNeurons(1, 2) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Hidden 3 Are Getting Set: ";
    std::cout << (generation.SetHiddenNeurons(2, 2) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Hidden 4 Are Getting Set: ";
    std::cout << (generation.SetHiddenNeurons(3, 2) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Hidden 5 Are Getting Set: ";
    std::cout << (generation.SetHiddenNeurons(4, 2) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Outputs  Are Getting Set: ";
    std::cout << (generation.SetOutputNeurons(1) ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Networks Are Getting Connected: ";
    std::cout << (generation.ConnectNetworks() ? "Successfull!" : "Failed!") << "\n";

    generation.Randomize();
    
    do
    {
        std::cout << "[-1] Test\n[-2] Best to File\n[-3] Exit\nAny Positive Number for train count\nDecision: ";
        std::cin >> decision;
        
        switch (decision)
        {
            case -3:
                std::cout << "Exiting...\n";
                break;
            case -2:
                generation.WriteBestToFile();
                break;
            default:
                for (trainCounter = 0; trainCounter < decision; trainCounter++)
                {
                    std::cout << (trainCounter + 1) << "\n";
                    for (inputCounter = 0; inputCounter < 10; inputCounter++)
                    {
                        generation.ResetScores();
                        for (groupSize = 0; groupSize < 3; groupSize++)
                        {
                            for (floatCounter = 0; floatCounter < 4; floatCounter++)
                                generation.SetInput(trainInputs[inputCounter * 3 + groupSize][floatCounter], floatCounter);
                            generation.SetTarget(trainInputs[inputCounter * 3 + groupSize][4]);
                            generation.Fire();
                            generation.UpdateScores();
                        }
                        generation.SortByScore();
                        generation.NextGeneration();
                    }
                }
                std::cout << "Best Score -> " << generation.GetBestPrediction() << "\n";
                std::cout << "Train is Over!\n";
                // break; To test it after the train is done
            case -1:
                outputFile = fopen("Data/results.data", "w");
                for (inputCounter = 0; inputCounter < 120; inputCounter++)
                {
                    for (floatCounter = 0; floatCounter < 4; floatCounter++)
                        generation.SetInput(testInputs[inputCounter][floatCounter], floatCounter);
                    generation.SetTarget(testInputs[inputCounter][4]);
                    
                    generation.Fire();
                    currentError = testInputs[inputCounter][4] - generation.GetBestPrediction() < 0 ? generation.GetBestPrediction() - testInputs[inputCounter][4] : testInputs[inputCounter][4] - generation.GetBestPrediction();
                    fprintf(outputFile, "%f,%f,%f\n", testInputs[inputCounter][4], generation.GetBestPrediction(), currentError);
                }
                fclose(outputFile);
                std::cout << "Test is Over!\n";
                break;
        }
    } while (decision != -3);

    return 0;
}
