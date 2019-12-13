#include <iostream>
#include <cstdlib>
#include <time.h>


float RandomFloat(int min, int max)
{
    static unsigned long int counter = 0;
    srand(time(0) + counter++ * 50);
    int value = (rand() % ((max - min) * 100));
    return (float)value / 100.0 + (float)min + 1.0;
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
            Sinaps(float, float, float); // Kaydedilen değerleri yeniden yazabilmek için
            void SetSinaps(float, float, float); // Sonradan tamamen değiştirebilmek için
            void SetWeight(float);
            void SetValue(float);
            void SetBias(float);
            float Fire();
    };

    Sinaps::Sinaps() { weight = value = bias = 0.0; }
    Sinaps::Sinaps(float weight, float value, float bias)
    {
        this -> weight = weight;
        this -> value = value;
        this -> bias = bias;
    }

    void Sinaps::SetSinaps(float weight, float value, float bias) {
        std::cout << "weight = " << weight << "\n";
        std::cout << "value = " << value << "\n";
        std::cout << "bias = " << bias << "\n";
        Sinaps(weight, value, bias); }
    void Sinaps::SetWeight(float weight) { this -> weight = weight; }
    void Sinaps::SetValue(float value) { std::cout << value << "\n"; this -> value = value; }
    void Sinaps::SetBias(float bias) { this -> bias = bias; }

    float Sinaps::Fire() { return weight * value + bias; }
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
    }
    
    Noron::~Noron()
    {
        delete forwards;
        delete incoming;
    }
    
    bool Noron::SetForwards(Sinaps *newForwards, int size)
    {
        forwards = (Sinaps *) new char[sizeof(Sinaps) * size];
        
        if(!forwards) return false;

        for (int i = 0; i < size; i++)
            *(forwards+i) = *(newForwards+i);
        
        forwardsCount = size;
        return true;
    }

    bool Noron::SetIncoming(Sinaps *newIncoming, int size)
    {
        incoming = (Sinaps *) new char[sizeof(Sinaps) * size];
        
        if(!incoming) return false;

        for (int i = 0; i < size; i++)
            *(incoming+i) = *(newIncoming+i);
        
        incomingCount = size;
        return true;
    }

    float Noron::GetStatus()
    {
        float toplam = 0.0;
        std::cout << toplam << "\n";
        for (int i = 0; i < incomingCount; i++)
            toplam += (incoming + i) -> Fire();

        for (int i = 0; i < forwardsCount; i++)
            (forwards + i) -> SetValue(toplam);

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
    
    Katman::Katman() { neurons = NULL; this -> size = 0; }
    Katman::Katman(int size) 
    { 
        Katman();
        
        if(!CreateNoron(size))
            std::cout << "Katman Oluşturulamadı!";
        else
            this -> size = size;
    }
    Katman::~Katman() { delete neurons; }

    
    Sinaps *Katman::CreateSinapsSet(int size)
    {
        Sinaps* sinapses = (Sinaps *) new char[sizeof(Sinaps) * size];

        if(sinapses)
            for (int i = 0; i < size; i++)
                *(sinapses + i) = Sinaps();
        
        return sinapses;
    }
    void Katman::RandomizeSinapsValues()
    {
        if(!forward)
            return;

        int sinapsCount = size * (forward -> GetSize());
        for (int i = 0; i < sinapsCount; i++)
        {
            (layerSinapses + i) -> SetSinaps(
                RandomFloat(-5, 5), 
                RandomFloat(-5, 5), 
                RandomFloat(-5, 5)
            );
        }
    }

    void Katman::FireLayer()
    {
        for (int i = 0; i < size; i++)
            std::cout << i << ". Fire = " << (neurons + i) -> GetStatus() << "\n";
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
            return true;

        forwardSize = forward -> GetSize();

        sinapses = (Sinaps *) new char[sizeof(Sinaps) * size * forwardSize];

        if(!sinapses)
            return false;

        // Set Forwards of each neuron in the Layer
        for (int thisCounter = 0; thisCounter < size; thisCounter++)
        {
            newSinapses = CreateSinapsSet(forwardSize);
            
            if(!newSinapses)
                return false;

            (neurons + thisCounter) -> SetForwards(newSinapses, forwardSize);
            
            // Add each sinaps to the array
            for (int forwardCounter = 0; forwardCounter < forwardSize; forwardCounter++)
                *(sinapses + (sinapsesIndex++)) = *(newSinapses + forwardCounter);
        }

        layerSinapses = sinapses;
        // Send the sinapses to the forward layer
        return forward -> SetIncoming(sinapses, size);
    }
    
    bool Katman::SetIncoming(Sinaps *sinapsSet, int backwardsNeuronCount)
    {
        Sinaps *sinapses = NULL;

        sinapses = (Sinaps *) new char[sizeof(Sinaps) * backwardsNeuronCount];
        
        if(!sinapses)
            return false;
        
        for (int thisCounter = 0; thisCounter < size; thisCounter++)
        {
            // Add each sinaps to the array
            for (int incomingCounter = 0; incomingCounter < backwardsNeuronCount; incomingCounter++)
                *(sinapses + (size * thisCounter + incomingCounter)) = *(sinapsSet + incomingCounter);

            (neurons + thisCounter) -> SetIncoming(sinapses, backwardsNeuronCount);
        }

        return true;
    }

    bool Katman::SetNoron(Noron *newneurons, int size)
    {
        neurons = (Noron *) new char[sizeof(Noron) * size];
        
        if(!neurons) return false;

        for (int i = 0; i < size; i++)
            *(neurons+i) = *(newneurons+i);
        
        this -> size = size;
        return true;
    }
    
    bool Katman::CreateNoron(int size)
    {
        neurons = (Noron *) new char[sizeof(Noron) * size];
        
        if(!neurons) return false;

        for (int i = 0; i < size; i++)
            *(neurons+i) = Noron();
        
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

            for (int i = 0; i < forwardNeuronCount; i++)
            {
                editedSinaps = (layerSinapses + index * forwardNeuronCount + i);
                editedSinaps -> SetValue(value); 
            }
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
            return (neurons + index) -> GetStatus();
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

        if(!input)
            std::cout << "Girdi Katmani Olusturulamadi!" << "\n";
        if(!hiddenLayers)
            std::cout << "Ara Katmanlar Olusturulamadi!" << "\n";
        if(!output)
            std::cout << "Cikti Katmani Olusturulamadi!" << "\n";
        
        if(!input || !hiddenLayers || !output)
            return;

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
        Noron *neurons = new Noron[size];
        
        if(!neurons)
            return false;
        
        return (hiddenLayers + index) -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::SetInputNeurons(int size)
    {
        Noron *neurons = new Noron[size];

        if(!neurons)
            return false;

        return output -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::SetOutputNeurons(int size)
    {
        Noron *neurons = new Noron[size];

        if(!neurons)
            return false;

        return input -> SetNoron(neurons, size);
    }

    bool NeuralNetwork::ConnectLayers()
    {
        if(!input -> SetForward(hiddenLayers))
            return false;
        
        for (int i = 0; i < hiddenSize - 1; i++)
            if(!(hiddenLayers + i) -> SetForward(hiddenLayers + i + 1))
                return false;
        
        if(!(hiddenLayers + hiddenSize - 1) -> SetForward(output))
            return false;

        return output -> SetForward(NULL);
    }

    void NeuralNetwork::FireNetwork()
    {
        input -> FireLayer();
        
        for (int i = 0; i < hiddenSize; i++)
            (hiddenLayers + i) -> FireLayer();
        
        output -> FireLayer();
    }

    void NeuralNetwork::SetInput(int index, float value)
    {
        if(!input)
            return;
        
        input -> SetValue(index, value);
    }

    void NeuralNetwork::RandomizeNetworkValues()
    {
        input -> RandomizeSinapsValues();

        for (int i = 0; i < hiddenSize; i++)
            (hiddenLayers + i) -> RandomizeSinapsValues();

        output -> RandomizeSinapsValues();
    }

    float NeuralNetwork::GetOutputValue(int index)
    {
        return output -> GetValue(index);
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
    network.RandomizeNetworkValues();

    network.SetInput(0, 1);
    std::cout << "m1\n";

    network.FireNetwork();
    std::cout << "m2\n";
    std::cout << network.GetOutputValue(0) << "\n";
    std::cout << "m3\n";





    // Katman k1(5);
    // Katman k2(3);

    // std::cout << "k1 SetForward = " << k1.SetForward(&k2) << "\n";
    // std::cout << "k2 SetForward = " << k2.SetForward(NULL) << "\n";
    // k1.FireLayer();
    // k2.FireLayer();
    return 0;
}
