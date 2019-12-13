#include <iostream>
#include <cstdlib>
#include <time.h>


float RandomFloat(int min, int max)
{
    float result;
    int value;
    static unsigned long int counter = 0;
    srand(time(0) + counter++ * 50);
    value = (rand() % ((max - min) * 100));
    result = (float)value / 100.0 + (float)min;
    std::clog << "Function RandomFloat: Between " << min << " and " << max << " returned value is " << result << "\n";
    return result;
}

#pragma region Sinaps 
    class Sinaps
    {
        private:
            float weight; // Ağırlık
            float value;  // Değer
            float bias;   // Öteleme
        public:
            Sinaps();
            ~Sinaps();
            Sinaps(float, float, float); // Kaydedilen değerleri yeniden yazabilmek için
            void SetSinaps(float, float, float); // Sonradan tamamen değiştirebilmek için
            void SetWeight(float);
            void SetValue(float);
            void SetBias(float);
            float Fire();
    };

    Sinaps::Sinaps() 
    { 
        weight = value = bias = 0.0; 
        std::clog << "Create Sinaps: Weight = " << weight
                  << " Value = " << value
                  << " Bias = " << bias << "\n";
    }
    
    Sinaps::~Sinaps() 
    {
        std::clog << "Delete Sinaps: Weight = " << weight
                  << " Value = " << value
                  << " Bias = " << bias << "\n";
    }
    Sinaps::Sinaps(float weight, float value, float bias)
    {
        this -> weight = weight;
        this -> value = value;
        this -> bias = bias;
        std::clog << "Create Sinaps: Weight = " << weight
                  << " Value = " << value
                  << " Bias = " << bias << "\n";
    }

    void Sinaps::SetSinaps(float weight, float value, float bias) 
    { 
        this -> weight = weight;
        this -> value = value;
        this -> bias = bias;
        std::clog << "Set Sinaps: Weight = " << weight
                  << " Value = " << value
                  << " Bias = " << bias << "\n";
     }
    void Sinaps::SetWeight(float weight) { std::clog << "Set Sinaps Weight: " << weight << "\n"; this -> weight = weight; }
    void Sinaps::SetValue(float value) { std::clog << "Set Sinaps Value: " << value << "\n"; this -> value = value; }
    void Sinaps::SetBias(float bias) { std::clog << "Set Sinaps Bias: " << bias << "\n"; this -> bias = bias; }

    float Sinaps::Fire() 
    { 
        float result = weight * value + bias;
        std::clog << "Return Sinaps Fire: " << weight << " * " << 
                                            value << " + " <<
                                            bias << " = " << 
                                            result << "\n"; 
        return result;
    }
#pragma endregion
#pragma region Noron
    class Noron
    {
        private:
            Sinaps *forwards;
            Sinaps *incoming;
            int forwardsCount;
            int incomingCount;
        public:
            Noron();
            ~Noron();
            bool SetForwards(Sinaps *, int);
            bool SetIncoming(Sinaps *, int);
            float GetStatus();
    };
    
    Noron::Noron()
    {
        forwards = incoming = NULL;
        forwardsCount = incomingCount = 0;
        std::clog << "Create Noron: NULL" << "\n";
    }
    
    Noron::~Noron()
    {
        delete forwards;
        delete incoming;
    }
    
    bool Noron::SetForwards(Sinaps *newForwards, int size)
    {
        std::clog << "SetForwards: Allocating Memory of Size " << size << "\n";
        forwards = (Sinaps *)new char[sizeof(Sinaps) * size];
        
        if(!forwards) 
        {
            std::clog << "SetForwards: Memory Couldn't Allocated!" << "\n";
            return false;
        }
        std::clog << "SetForwards: Memory Allocated!" << "\n";

        for (int i = 0; i < size; i++)
            *(forwards+i) = *(newForwards+i);
        
        forwardsCount = size;
        std::clog << "SetForwards: Successfull!" << "\n";
        return true;
    }

    bool Noron::SetIncoming(Sinaps *newIncoming, int size)
    {
        std::clog << "SetIncoming: Allocating Memory of Size " << size << "\n";
        incoming = (Sinaps *)new char[sizeof(Sinaps) * size];
        
        if(!incoming) 
        {
            std::clog << "SetIncoming: Memory Couldn't Allocated!" << "\n";
            return false;
        }
        std::clog << "SetIncoming: Memory Allocated!" << "\n";

        for (int i = 0; i < size; i++)
            *(incoming+i) = *(newIncoming+i);
        
        incomingCount = size;
        std::clog << "SetIncoming: Successfull!" << "\n";
        return true;
    }

    float Noron::GetStatus()
    {
        float toplam = 0.0;
        
        for (int i = 0; i < incomingCount; i++)
            toplam += (incoming + i) -> Fire();

        for (int i = 0; i < forwardsCount; i++)
            (forwards + i) -> SetValue(toplam);

        std::clog << "Get Noron Status: Sum = " << toplam << "\n";
        return toplam;
    }
#pragma endregion
#pragma region Katman
    class Katman
    {
        protected:
            Noron *neurons;
            Katman *forward;
            Sinaps *layerSinapses;
            int size;
            Sinaps *CreateSinapsSet(int size);
        public:
            Katman();
            Katman(int);
            ~Katman();
            void FireLayer();
            void RandomizeSinapsValues();
            bool SetForward(Katman *);
            bool SetIncoming(Sinaps *sinapsSet, int backwardsNeuronCount);
            bool SetNoron(Noron *, int);
            bool CreateNoron(int);
            int GetSize();
    };
    
    Katman::Katman() 
    {
        neurons = NULL; this -> size = 0; 
        std::clog << "Create Layer: NULL" << "\n";
    }
    Katman::Katman(int size) 
    { 
        Katman();
        
        if(!CreateNoron(size))
        {
            std::clog << "Error Create Layer: Neurons Couldn't Created!" << "\n";
            std::cout << "Katman Oluşturulamadı!";
            return;
        }

        std::clog << "Create Layer: " << size << " Neurons Has Been Created!" << "\n";
        this -> size = size;
    }
    Katman::~Katman() { delete neurons; }

    
    Sinaps *Katman::CreateSinapsSet(int size)
    {
        Sinaps* sinapses = new Sinaps[size];

        if(sinapses)
            std::clog << "Create Sinaps Set: " << size << " Sinapses Has Been Created!" << "\n";
        else
            std::clog << "Error Create Sinaps Set!" << "\n";
        
        return sinapses;
    }
    void Katman::RandomizeSinapsValues()
    {
        if(!forward)
            return;

        float weight;
        float value;
        float bias;

        int sinapsCount = size * (forward -> GetSize());
        for (int i = 0; i < sinapsCount; i++)
        {
            weight = RandomFloat(-1, 1);
            value = RandomFloat(-1, 1);
            bias = RandomFloat(-1, 1);

            (layerSinapses + i) -> SetSinaps(weight, value, bias);
            std::clog << "Call RandomizeSinapsValues: SetSinaps Called With Values of SetSinaps(" << weight << ", " << value << ", " << bias << ")" << "\n";
        }
    }

    void Katman::FireLayer()
    {
        std::clog << "Call FireLayer: Number of " << size << " Neurons' GetStatus is Being Called!" << "\n";
        for (int i = 0; i < size; i++)
            std::clog << i << ". Neuron Status: " << (neurons + i) -> GetStatus() << "\n";
    }

    bool Katman::SetForward(Katman *forward)
    { 
        Sinaps *sinapses = NULL; // Pointer to store all created sinapses
        Sinaps *newSinapses = NULL; // Temporary Pointer for creating each neurons' s1inapses
        int forwardSize;
        int sinapsesIndex = 0;
        delete layerSinapses;

        this -> forward = forward;
        
        if(!forward)
        {
            std::clog << "Call SetForward: Forward is NULL" << "\n";
            return true;
        }

        forwardSize = forward -> GetSize();
        std::clog << "Call SetForward: Creating Sinaps Set with Number of " << (size * forwardSize) << "\n";
        sinapses = (Sinaps *)new char[sizeof(Sinaps) * size * forwardSize];

        if(!sinapses)
        {
            std::clog << "Error Call SetForward: Couldn't Allocate Memory for Sinapses!" << "\n";
            return false;
        }
        std::clog << "Call SetForward: Sinapses Set Created!" << "\n";

        // Set Forwards of each neuron in the Layer
        for (int thisCounter = 0; thisCounter < size; thisCounter++)
        {
            newSinapses = CreateSinapsSet(forwardSize);
            
            if(!newSinapses)
            {
                std::clog << "Call SetForward -> CreateSinapsSet: Couldn't Allocate Memory for Sinapses!" << "\n";
                return false;
            }
            std::clog << "Call SetForward -> CreateSinapsSet: Sinapses Set Created!" << "\n";

            std::clog << "Call SetForward -> SetForwards: " << 
                (neurons + thisCounter) -> SetForwards(newSinapses, forwardSize) 
            << "\n";;
            
            // Add each sinaps to the array
            for (int forwardCounter = 0; forwardCounter < forwardSize; forwardCounter++)
                *(sinapses + (sinapsesIndex++)) = *(newSinapses + forwardCounter);
            std::clog << "Call SetForward: Sinapses Are Added to the Array!" << "\n";
        }

        layerSinapses = sinapses;
        std::clog << "Call SetForward: Sinaps Array Has Been Set to the Class' Pointer!" << "\n";
        // Send the sinapses to the forward layer
        return forward -> SetIncoming(sinapses, size);
    }
    
    bool Katman::SetIncoming(Sinaps *sinapsSet, int backwardsNeuronCount)
    {
        Sinaps *sinapses = NULL;

        std::clog << "Call SetIncoming: Creating Sinaps Set with Number of " << backwardsNeuronCount << "\n";
        sinapses = (Sinaps *)new char[sizeof(Sinaps) * backwardsNeuronCount];
        
        if(!sinapses)
        {
            std::clog << "Error Call SetIncoming: Couldn't Allocate Memory for Sinapses!" << "\n";
            return false;
        }
        std::clog << "Call SetIncoming: Sinapses Set Created!" << "\n";
        
        for (int thisCounter = 0; thisCounter < size; thisCounter++)
        {
            std::clog << "Call SetIncoming: Sinapses Are Being Added to the Array!" << "\n";
            // Add each sinaps to the array
            for (int incomingCounter = 0; incomingCounter < backwardsNeuronCount; incomingCounter++)
                *(sinapses + (size * thisCounter + incomingCounter)) = *(sinapsSet + incomingCounter);

            std::clog << "Call SetIncoming -> Neuron SetIncoming: " << 
                (neurons + thisCounter) -> SetIncoming(sinapses, backwardsNeuronCount)
            << "\n";
        }

        return true;
    }

    bool Katman::SetNoron(Noron *newNeurons, int size)
    {
        std::clog << "Call SetNoron: Creating Neurons with Number of " << size << "\n";
        neurons = (Noron *) new char[sizeof(Noron) * size];
        
        if(!neurons)
        {
            std::clog << "Error Call SetNoron: Creating Neurons Failed!" << "\n";
            return false;
        }
        std::clog << "Call SetNoron: Neurons Created Successfully!" << "\n";

        std::clog << "Call SetNoron: Setting Neurons to the Class' Neurons!" << "\n";
        for (int i = 0; i < size; i++)
            *(neurons+i) = *(newNeurons+i);
        
        std::clog << "Call SetNoron: Neurons are Set Successfully!" << "\n";
        this -> size = size;
        return true;
    }
    
    bool Katman::CreateNoron(int size)
    {
        std::clog << "Call CreateNoron: Creating Neurons with Number of " << size << "\n";

        neurons = new Noron[size];
        
        if(!neurons)
        {
            std::clog << "Error Call CreateNoron: Creating Neurons Failed!" << "\n";
            return false;
        }
        std::clog << "Call CreateNoron: Neurons Created Successfully!" << "\n";

        std::clog << "Call CreateNoron: Neurons are Set Successfully!" << "\n";
        this -> size = size;
        return true;
    }

    int Katman::GetSize() { return size; }
#pragma endregion
#pragma region Girdi-Cikti
    #pragma region Girdi
        class Girdi : public Katman
        {
            public:
                Girdi();
                Girdi(int);
                void SetValue(int, float);
        };

        Girdi::Girdi() : Katman() {}
        Girdi::Girdi(int size) : Katman(size) {}

        void Girdi::SetValue(int index, float value)
        {
            Sinaps *editedSinaps = NULL;
            int forwardNeuronCount = forward -> GetSize();
            std::clog << "Call SetValue: Index of " << index << " Neuron's Sinapses Values are Getting Set to Value of " << value << "\n";

            for (int i = 0; i < forwardNeuronCount; i++)
            {
                std::clog << "Call SetValue -> Sinaps SetValue: Index of " << index << " Neuron's " << i << " Sinaps Value is Getting Set to Value of " << value << "\n";
                editedSinaps = (layerSinapses + index * forwardNeuronCount + i);
                editedSinaps -> SetValue(value); 
                std::clog << "Call SetValue -> Sinaps SetValue: Successfull" << "\n";
            }
            std::clog << "Call SetValue: Successfull" << "\n";
        }
    #pragma endregion
    #pragma region Cikti
        class Cikti : public Katman
        {
            public:
                Cikti();
                Cikti(int);
                float GetValue(int);
        };

        Cikti::Cikti() : Katman() {}
        Cikti::Cikti(int size) : Katman(size) {}

        float Cikti::GetValue(int index)
        {
            float result = (neurons + index) -> GetStatus();
            std::clog << "Call GetValue: " << result << "\n";
            return result;
        }
    #pragma endregion
#pragma endregion
#pragma region NeuralNetwork
    class NeuralNetwork
    {
        private:
            Girdi *input;
            Katman *hiddenLayers;
            Cikti *output;
            int hiddenSize;
        public:
            NeuralNetwork();
            NeuralNetwork(int);
            ~NeuralNetwork();
            void SetInput(int, float);
            void RandomizeNetworkValues();
            void FireNetwork();
            bool SetInputNeurons(int);
            bool SetHiddenLayerNeurons(int, int);
            bool SetOutputNeurons(int);
            bool ConnectLayers();
            float GetOutputValue(int);
    };
    
    NeuralNetwork::NeuralNetwork()
    {
        std::clog << "Create NeuralNetwork: NULL" << "\n";
        hiddenSize = 0;
        input = NULL;
        hiddenLayers = NULL;
        output = NULL;
    }
    
    NeuralNetwork::NeuralNetwork(int hiddenSize)
    {
        input = new Girdi();
        hiddenLayers = new Katman[hiddenSize];
        output = new Cikti();
        std::clog << "Create NeuralNetwork: New Neural Network Created with " << hiddenSize << " Layers!" << "\n";

        if(!input)
        {
            std::clog << "Error Create NeuralNetwork: Memory Couldn't Allocated for Input Layer!" << "\n";
            std::cout << "Girdi Katmani Olusturulamadi!" << "\n";
        }
        if(!hiddenLayers)
        {
            std::clog << "Error Create NeuralNetwork: Memory Couldn't Allocated for Hidden Layers!" << "\n";
            std::cout << "Ara Katmanlar Olusturulamadi!" << "\n";
        }
        if(!output)
        {
            std::clog << "Error Create NeuralNetwork: Memory Couldn't Allocated for Output Layer!" << "\n";
            std::cout << "Cikti Katmani Olusturulamadi!" << "\n";
        }
        
        if(!input || !hiddenLayers || !output)
            return;

        std::clog << "Create NeuralNetwork: Succesfull!" << "\n";
        this -> hiddenSize = hiddenSize;
    }
    
    NeuralNetwork::~NeuralNetwork()
    {
        delete input;
        delete hiddenLayers;
        delete output;
    }

    bool NeuralNetwork::SetHiddenLayerNeurons(int index, int size)
    {
        std::clog << "Call SetHiddenLayerNeurons: Size of " << size << " at Index of " << index << "\n";
        std::clog << "Call SetHiddenLayerNeurons: Creating " << size << " Neurons!" << "\n";
        Noron *neurons = (Noron *) new char[sizeof(Noron) * size];
        
        if(!neurons)
        {
            std::clog << "Error Call SetHiddenLayerNeurons: Couldn't Allocate Memory for Neurons!" << "\n";
            return false;
        }
        std::clog << "Call SetHiddenLayerNeurons: Neurons Are Created!" << "\n";
        
        return (hiddenLayers + index) -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::SetInputNeurons(int size)
    {
        std::clog << "Call SetInputNeurons: Size of " << size << "\n";
        std::clog << "Call SetInputNeurons: Creating " << size << " Neurons!" << "\n";
        Noron *neurons = (Noron *) new char[sizeof(Noron) * size];

        if(!neurons)
        {
            std::clog << "Error Call SetInputNeurons: Couldn't Allocate Memory for Neurons!" << "\n";
            return false;
        }
        std::clog << "Call SetInputNeurons: Neurons Are Created!" << "\n";

        return output -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::SetOutputNeurons(int size)
    {
        std::clog << "Call SetInputNeurons: Size of " << size << "\n";
        std::clog << "Call SetInputNeurons: Creating " << size << " Neurons!" << "\n";
        Noron *neurons = (Noron *) new char[sizeof(Noron) * size];

        if(!neurons)
        {
            std::clog << "Error Call SetInputNeurons: Couldn't Allocate Memory for Neurons!" << "\n";
            return false;
        }
        std::clog << "Call SetInputNeurons: Neurons Are Created!" << "\n";

        return input -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::ConnectLayers()
    {
        if(!input -> SetForward(hiddenLayers))
        {
            std::clog << "Call ConnectLayers: Input Couldn't Set to Forward!" << "\n";
            return false;
        }
        std::clog << "Call ConnectLayers: Input is Set to Forward Successfully!" << "\n";
        
        for (int i = 0; i < hiddenSize - 1; i++)
            if(!(hiddenLayers + i) -> SetForward(hiddenLayers + i + 1))
            {
                std::clog << "Call ConnectLayers: Hidden Layer " << i << " Couldn't Set to Forward!" << "\n";
                return false;
            }
        std::clog << "Call ConnectLayers: Hidden Layers are Set to Forward Successfully!" << "\n";
        
        if(!(hiddenLayers + hiddenSize - 1) -> SetForward(output))
        {
            std::clog << "Call ConnectLayers: Output Couldn't Set to Forward!" << "\n";
            return false;
        }
        std::clog << "Call ConnectLayers: Output is Set to Forward Successfully!" << "\n";
        
        return output -> SetForward(NULL);
    }

    void NeuralNetwork::FireNetwork()
    {
        // input -> FireLayer();
        // std::clog << "Call FireNetwork: Input Fired!" << "\n";
        
        for (int i = 0; i < hiddenSize; i++)
        {
            (hiddenLayers + i) -> FireLayer();
            std::clog << "Call FireNetwork: Hidden Layer " << i << " Fired!" << "\n";
        }
        
        output -> FireLayer();
        std::clog << "Call FireNetwork: Output Fired!" << "\n";
    }

    void NeuralNetwork::SetInput(int index, float value)
    {
        if(!input)
        {
            std::clog << "Call SetInput: There's no Input Layer Set!" << "\n";
            return;
        }

        std::clog << "Call SetInput -> Input SetValue: SetValue(" << index << ", " << value << ")!" << "\n";
        input -> SetValue(index, value);
    }

    void NeuralNetwork::RandomizeNetworkValues()
    {
        std::clog << "Call RandomizeNetworkValues: Input Sinapses Are Getting Randomized!" << "\n";
        input -> RandomizeSinapsValues();

        for (int i = 0; i < hiddenSize; i++)
        {
            std::clog << "Call RandomizeNetworkValues: Hidden Layer " << i << " Sinapses Are Getting Randomized!" << "\n";
            (hiddenLayers + i) -> RandomizeSinapsValues();
        }

        std::clog << "Call RandomizeNetworkValues: Output Sinapses Are Getting Randomized!" << "\n";
        output -> RandomizeSinapsValues();
    }

    float NeuralNetwork::GetOutputValue(int index)
    {
        float result = output -> GetValue(index);
        std::clog << "Call GetOutputValue: " << result << "\n";
        return result;
    }
#pragma endregion
int main(int argc, char const *argv[])
{
    NeuralNetwork network(3);

    network.SetInputNeurons(1);
    network.SetHiddenLayerNeurons(0, 2);
    network.SetHiddenLayerNeurons(1, 3);
    network.SetHiddenLayerNeurons(2, 2);
    network.SetOutputNeurons(1);

    network.ConnectLayers();
    std::cout << "m1\n";
    network.RandomizeNetworkValues();
    std::cout << "m2\n";

    // network.SetInput(0, 1);
    std::cout << "m3\n";

    // network.FireNetwork();
    // std::cout << network.GetOutputValue(0) << "\n";
    return 0;
}
