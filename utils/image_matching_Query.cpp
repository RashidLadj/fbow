// //loads a vocabulary, and a image. Extracts image feaures and then  compute the bow of the image
// #include "fbow.h"
// #include <iostream>
// #include <map>
// using namespace std;

// // OpenCV
// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/features2d/features2d.hpp>

// #ifdef USE_CONTRIB
//     #include <opencv2/xfeatures2d/nonfree.hpp>
//     #include <opencv2/xfeatures2d.hpp>
// #endif

// #include "dirreader.h"

// #include <filesystem>

// using namespace std::__fs::filesystem;

// #include <chrono>
// class CmdLineParser { int argc; char **argv; public: CmdLineParser(int _argc, char **_argv) :argc(_argc), argv(_argv) {}  bool operator[] (string param) { int idx = -1;  for (int i = 0; i<argc && idx == -1; i++) if (string(argv[i]) == param) idx = i;    return (idx != -1); } string operator()(string param, string defvalue = "-1") { int idx = -1;    for (int i = 0; i<argc && idx == -1; i++) if (string(argv[i]) == param) idx = i; if (idx == -1) return defvalue;   else  return (argv[idx + 1]); } };

// vector< cv::Mat > loadFeatures(string path_to_image, string descriptor = "")  {
//     //select detector
//     cv::Ptr<cv::Feature2D> fdetector;
//     if (descriptor == "orb")        fdetector = cv::ORB::create(2000);
//     else if (descriptor == "brisk") fdetector = cv::BRISK::create();
// #ifdef OPENCV_VERSION_3
//     else if (descriptor == "akaze") fdetector = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_MLDB, 0, 3, 1e-4);
// #endif
// #ifdef USE_CONTRIB
//     else if (descriptor == "surf")  fdetector = cv::xfeatures2d::SURF::create(15, 4, 2);
// #endif

//     else throw std::runtime_error("Invalid descriptor");
//     assert(!descriptor.empty());
//     vector< cv::Mat > features;

//     cout << "Extracting features for " << path_to_image << " ... " << endl;
//     vector<cv::KeyPoint> keypoints;
//     cv::Mat descriptors;
//     // cout << "reading image: " << path_to_images[i] << endl;
//     cv::Mat image = cv::imread(path_to_image, 0);
//     if (image.empty()) throw std::runtime_error("Could not open image" + path_to_image);
//     // cout << "extracting features" << endl;
//     fdetector->detectAndCompute(image, cv::Mat(), keypoints, descriptors);
//     features.push_back(descriptors);
//     cout << "done extracting features " << endl;

//     return features;
// }

// int main(int argc, char **argv) {
//     CmdLineParser cml(argc, argv);
//     try {
//         if (argc < 4 || cml["-h"]) throw std::runtime_error("Usage: FBoW_File Output_File(Ne sert à rien pour le moment) Image1 Image2 ... [descriptor]\nUsage: FBoW_File Output_File(Ne sert à rien pour le moment) Images_Directory [descriptor]\n");
//         /** cd ..; cmake ..; make;  cd utils; ./image_matching ../../vocabularies/orb_mur.fbow output_image_matching 
//          * images/poly01.jpg images/poly02.jpg images/poly03.jpg images/poly04.jpg images/poly05.jpg images/poly06.jpg 
//          * images/poly07.jpg images/poly08.jpg images/poly09.jpg images/poly10.jpg images/poly11.jpg images/poly12.jpg 
//          * images/poly13.jpg images/poly14.jpg images/poly15.jpg images/poly16.jpg images/poly17.jpg 
//          * **/
//         /** cd ..; cmake ..; make;  cd utils; ./image_matching ../../vocabularies/orb_mur.fbow output_image_matching imagesFolder **/
        
//         /** Lecture du vocabulaire **/
//         fbow::Vocabulary voc;
//         voc.readFromFile(argv[1]);

//         /** Recuperation du Descripteur (ORB, Brisk, Surf, Akaze) **/
//         string desc_name = voc.getDescName();
//         cout << "voc desc name=" << desc_name << endl;

//         vector<vector<cv::Mat> > features;
//         vector<map<double, int> > scores;
//         string outDir = argv[2];

//         vector<string> filenames;

//         /** read Images From Folder **/
//         if (argc == 4){ 
//             for (const auto & entry : directory_iterator(argv[3])){
//                 path tmp = entry.path();
//                 /** Chekck Extension of File Before add in list **/
//                 if (tmp.extension() == ".jpg" || tmp.extension() == ".png")
//                     filenames.push_back(entry.path());
//             }
//         }
//         /** read Image per Image From CommandLine **/
//         else{           
//             for (int i = 3; i < argc; ++i)
//                 filenames.push_back(argv[i]);
//         }
        
//         /** Sort images filename **/
//         sort(filenames.begin(), filenames.end());
//         for (size_t i = 0; i<filenames.size(); ++i)
//             cout << "->" << filenames[i] << endl;

//         for (size_t i = 0; i<filenames.size(); ++i)
//             features.push_back(loadFeatures({ filenames[i] }, desc_name));
        
//         fbow::fBow vv, vv2;
//         int avgScore = 0;
//         int counter = 0;
//         auto t_start = std::chrono::high_resolution_clock::now();
//         for (size_t i = 0; i<features.size(); ++i){
//             vv = voc.transform(features[i][0]);
//             map<double, int> score;
//             for (size_t j = 0; j<features.size(); ++j){
//                 vv2 = voc.transform(features[j][0]);
//                  /** using L2_Norm , can't modify, need Implement Another if  would to test **/
//                 double score1 = vv.score(vv, vv2);   
//                 counter++;
//                 //		if(score1 > 0.01f)
//                 {
//                     score.insert(pair<double, int>(score1, j));
//                 }
//                 /** printf("%f, ", score1); **/
//             }
//             /** printf("\n"); **/
//             scores.push_back(score);
//         }
//         auto t_end = std::chrono::high_resolution_clock::now();
//         avgScore += double(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count());
        
//         /** Recover the most similar image among the images provided **/
//         for (size_t i = 0; i < scores.size(); i++){
//             auto it = (--scores[i].end());  /** L'avant dernier element, car le dernier element c'est l'image elle meme donc Score = 1 **/ 
//             cout << "Best Match of Image " << filenames[i] << "  is " << endl;
//             cout << "\t1- " << filenames[(--it)->second] << " with score of " << it->first << endl;
//             cout << "\t2- " << filenames[(--it)->second] << " with score of " << it->first << endl;
//         }

//         std::cout << "avg score: " << avgScore << " # of features: " << features.size() << std::endl;
//     }
//     catch (std::exception &ex) {
//         cerr << ex.what() << endl;
//     }

// }
