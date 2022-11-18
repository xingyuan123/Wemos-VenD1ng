# Wemos-VenD1ng
data collected by jun heng, chuan an and xing yuan  
machine learning models written by chuan an, xing yuan 

## Folder for machine learning
Link to dataset and CNN model due to size limit: 
CNN model: https://drive.google.com/drive/folders/1C0bOsZ0EkWXyLXLRA44dv00ZO2AriWii
Dataset: https://drive.google.com/drive/folders/1-QEtSv9Y_mRYQq9B9UQdSs1ABG4Amc89

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
KNN model. Test accuracy: 93.5%. Train accuracy: 96.9%

### rf.pkl
<<<<<<< HEAD
Random Forest model. Test accuracy: 89.6%

