import numpy as np
import pandas as pd
from sefr import SEFR
from sklearn.model_selection import train_test_split
from micromlgen import port

Train = pd.read_csv(r'C:\Users\David\Desktop\posture_td.csv')

# X_data is features, Y_data is label
X = np.array(Train.iloc[:, :6])
y = np.array(Train.iloc[:, 6])
y.transpose()
print("X")
print(X)
print("y")
print(y)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3)
# print(X_train)
# print(y_train)

ML = SEFR()
ML.fit(X_train, y_train)
print('accuracy', (ML.predict(X_test) == y_test).sum() / len(y_test))
c_code = port(ML)
print(c_code)
