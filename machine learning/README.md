# Wemos-VenD1ng

## Folder for machine learning

### dataset
Folder containing coin images used in model training and validation.

### ML_final.ipynb
Code snippets that will be used for coins prediction in the cloud server. 

### test_model.ipynb
Tests the models we have trained using unseen data to see how they fare. 
mapping_label = {'10c':0,'20c':1,'5c':2}
10 images, expected results: 1,1,1,1,0,0,0,0,2

### CNN.ipynb
Code for training of CNN model.

### HoughCircles_KNN.ipynb
Code for training of KNN model.

### RF.ipynb
Code for training of RF model.

### cnn-combined_updated.hd5
Convolutional Neural Network model. Test accuracy: 88.3%, loss: 0.47

### knn_hough.pkl
KNN model. Test accuracy: 94.8%

### rf.pkl
Random Forest mpdel. Test accuracy: 89.6%