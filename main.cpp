#include "Genetic.hpp"

int main()
{
    FILE *inputFile;
    FILE *outputFile;
    int decision;
    
    int trainCounter;
    int inputCounter;
    int doubleCounter;
    int groupCounter;

    double trainInputs[30][5];
    double testInputs[120][5];
    double currentError;
    Generation generation(50, 5);

    inputFile = fopen("Data/train.data", "r");
    for (inputCounter = 0; inputCounter < 30; inputCounter++)
        for (doubleCounter = 0; doubleCounter < 5; doubleCounter++)
            fscanf(inputFile, "%lf,", &trainInputs[inputCounter][doubleCounter]);
    fclose(inputFile);

    inputFile = fopen("Data/test.data", "r");
    for (inputCounter = 0; inputCounter < 120; inputCounter++)
        for (doubleCounter = 0; doubleCounter < 5; doubleCounter++)
            fscanf(inputFile, "%lf,", &testInputs[inputCounter][doubleCounter]);
    fclose(inputFile);

    std::cout << "Inputs   Are Getting Set: ";
    std::cout << (generation.SetInputNeurons(4)     ? "Successfull!" : "Failed!") << "\n";
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
    std::cout << (generation.SetOutputNeurons(1)    ? "Successfull!" : "Failed!") << "\n";
    std::cout << "Networks Are Getting Connected: ";
    std::cout << (generation.ConnectNetworks()      ? "Successfull!" : "Failed!") << "\n";

    std::cout << "Networks Are Getting Randomized: ";
    generation.Randomize();
    std::cout << "Done!\n";
    
    do
    {
        std::cout << "\n[-1] Test\n[-2] Best to File\n[-3] Exit\nAny Positive Number for train count\nDecision: ";
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
                        for (groupCounter = 0; groupCounter < 3; groupCounter++)
                        {
                            for (doubleCounter = 0; doubleCounter < 4; doubleCounter++)
                                generation.SetInput(trainInputs[inputCounter * 3 + groupCounter][doubleCounter], doubleCounter);
                            generation.SetTarget(trainInputs[inputCounter * 3 + groupCounter][4]);
                            generation.Fire();
                            generation.UpdateScores();
                        }
                        generation.SortByScore();
                        generation.NextGeneration();
                    }
                }
                std::cout << "Best Score -> " << generation.GetPredictionOfBestNetwork() << "\n";
                std::cout << "Train is Over!\n";
                // break; To test it after the train is done
            case -1:
                outputFile = fopen("Data/results.data", "w");
                for (inputCounter = 0; inputCounter < 120; inputCounter++)
                {
                    for (doubleCounter = 0; doubleCounter < 4; doubleCounter++)
                        generation.SetInput(testInputs[inputCounter][doubleCounter], doubleCounter);
                    generation.SetTarget(testInputs[inputCounter][4]);
                    
                    generation.Fire();
                    currentError = testInputs[inputCounter][4] - generation.GetPredictionOfBestNetwork() < 0 ? generation.GetPredictionOfBestNetwork() - testInputs[inputCounter][4] : testInputs[inputCounter][4] - generation.GetPredictionOfBestNetwork();
                    fprintf(outputFile, "%lf,%lf,%lf\n", testInputs[inputCounter][4], generation.GetPredictionOfBestNetwork(), currentError);
                }
                fclose(outputFile);
                std::cout << "Test is Over!\n";
                break;
        }
    } while (decision != -3);

    return 0;
}
