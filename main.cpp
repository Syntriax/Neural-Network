#include <iostream>
#include <time.h>

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
    static unsigned long int counter = 0;
    srand(time(0) + counter++ * 50);
    value = (rand() % ((max - min) * 100));
    result = (float)value / 100.0 + (float)min;
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
        this -> value = this -> weight = this -> bias = 1.0;
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
            float value;
        public:
            Neuron();
            void ConnectIncomings(Synapse *, int);
            void ConnectForwards(Synapse *, int, int);
            void SetValue(float);
            float GetValue();
    };
    
    Neuron::Neuron()
    {
        incomings = forwards = NULL;
        incomingsSize = forwardsSize = layerSize = 0;
        value = 0.0;
    }
    
    void Neuron::SetValue(float value)
    {
        this -> value = value;
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

        if(!incomings) return (value = result);

        for (int i = 0; i < incomingsSize; i++)
            result += (incomings + i) -> Fire();


        if(!forwards) return (value = result);

        for (int i = 0; i < forwardsSize; i++)
            // currentSynapse = (forwards -> synapses + (forwardNeuron * this -> neuronSize)); 
            // (forwards + i) -> SetValue(result);
            //BAK BURAYA
            (forwards + i * layerSize) -> SetValue(result);

        value = result;
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
        public:
            Layer();
            Layer(int);
            ~Layer();
            void FireLayer();
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
        newNeurons = (Neuron *) new char[sizeof(Neuron) * size];
        
        if(newNeurons)
            for (int i = 0; i < size; i++)
                *(newNeurons + i) = Neuron();

        return newNeurons;
    }

    void Layer::FireLayer()
    {
        for (int i = 0; i < neuronSize; i++)
            (neurons + i) -> GetValue();
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
        // Synapse *connectSynapses = NULL;

        if(synapses) delete synapses;
        synapses = (Synapse *) new char[sizeof(Synapse) * synapseCount];
        if(!synapses) return false;

        // connectSynapses = (Synapse *) new char[sizeof(Synapse) * previousSize];

        for (int thisNeuron = 0; thisNeuron < this -> neuronSize; thisNeuron++)
        {
            for (int prevNeuron = 0; prevNeuron < previousSize; prevNeuron++)
            {
                currentIndex = thisNeuron * previousSize + prevNeuron;
                currentSynapse = (synapses + currentIndex);
                currentNeuron = (previous -> neurons) + prevNeuron;
                
                *currentSynapse = Synapse();
                // currentSynapse = (Synapse *) new char[sizeof(Synapse)];
                // currentSynapse -> SetWeight(1);
                // currentSynapse -> SetValue(2);
                // currentSynapse -> SetBias(3);
                // currentSynapse -> SetRoot(currentNeuron);
            }

            currentNeuron = (neurons + thisNeuron);
            currentNeuron -> ConnectIncomings((synapses + thisNeuron * previousSize), previousSize);
        }

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
                // currentSynapse = (forwards -> synapses + (thisNeuron + forwardNeuron * this -> neuronSize)); 
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
                void SetValue(int, float);
        };
        
        Input::Input() : Layer() {}
        void Input::SetValue(int index, float value)
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
        float Output::GetValue(int index)
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
            bool SetInputNeurons(int);
            bool SetHiddenNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            float GetOutput(int);
            void SetInput(int, float);
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
        input = new Input();
        hidden = new Layer(hiddenSize);
        output = new Output();
    }
    
    NeuralNetwork::~NeuralNetwork()
    {
        if(input)   delete input;
        if(hidden)  delete hidden;
        if(output)  delete output;
    }

    void NeuralNetwork::FireNetwork()
    {
        for (int i = 0; i < hiddenSize; i++)
            (hidden + i) -> FireLayer();

        output -> FireLayer();
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

        if(output -> ConnectPrevious((hidden + hiddenSize - 1)))
            return false;
        
        return true;
    }
    
    float NeuralNetwork::GetOutput(int index)
    {
        return output -> GetValue(index);
    }
    
    void NeuralNetwork::SetInput(int index, float value)
    {
        input -> SetValue(index, value);
    }
#pragma endregion


int main(int argc, char const *argv[])
{
    NeuralNetwork network(3);

    network.SetInputNeurons(1);
    network.SetHiddenNeurons(0, 2);
    network.SetHiddenNeurons(1, 3);
    network.SetHiddenNeurons(2, 2);
    network.SetOutputNeurons(1);

    network.ConnectLayers();
    network.SetInput(0, 2);
    network.FireNetwork();
    std::cout << "Result = " << network.GetOutput(0) << "\n";
    return 0;
}
