"""
Aritificial Intelligence Basics @ DGIST
Homework, Part 1
We also provide the Jupyter notebook for this python file. See the pdf guideline.
"""
import numpy as np
import pandas as pd
import re

from collections import defaultdict
from sklearn.model_selection import train_test_split
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB, BernoulliNB, ComplementNB


############################################################################################################
# Part 1 - Utilities to handle datasets.
# We provide the complete implementations for this utilities here, so you would not need to modify this part.
"""
Data loader for the given CSV file
"""
def read_csv():
    df = pd.read_csv("spam.csv", encoding = 'ISO-8859-1')
    df.dropna(inplace = True, axis = 1)
    df['is_spam'] = df.v1.map(lambda x: 1 if x =='spam' else 0)
    df.head()

    subset = df[['v2', 'is_spam']]
    tuples = [tuple(x) for x in subset.values]
    
    # Note: we here fix the random_state for comparison with other models like scikit-learn algorithms
    train_dataset, test_dataset = train_test_split(tuples, test_size = 0.3, random_state = 1)
    return train_dataset, test_dataset


"""
Tokenize the content word by word with spaces for an email
"""
def tokenize(content):
    content = content.lower()
    word = re.findall("[a-z0-9']+", content)
    return set(word)

"""
Get all words in a dataset
"""
def tokenize_dataset(dataset):
    words_set = set()
    for content, _ in dataset:
        words_set = words_set.union(tokenize(content))

    return list(words_set)
############################################################################################################


############################################################################################################
# Part 2 - Probability Model for the training dataset
# You must complete the implementation of the following class.
"""
The Model class that models the probability values during training
"""
class NBModel:
    def __init__(self, train_dataset):
        self.train_dataset = train_dataset
        self.all_words_list = tokenize_dataset(train_dataset)
        self.spam_mail_list = [content for content, label in train_dataset if label == 1]
        self.ham_mail_list = [content for content, label in train_dataset if label == 0]
    """
    Task 1. return if a word was presented in the training dataset
    """
    def word_exists(self, word):
        return word in self.all_words_list

    """
    Task 2. P(S)
    Return the probability (0.0~1.0) that an email is a spam
    """
    def spam_prob(self):
        num_spam = sum(1 for _, label in self.train_dataset if label == 1)
        total_emails = len(self.train_dataset)
        return num_spam / total_emails
    
    """
    Task 3. P(word_i|S=spam)
    Return the conditional probability (0.0~1.0) that a spam email has the given word
    """
    def spam_cond_prob(self, word):
        num_spam_with_word = sum(1 for content, label in self.train_dataset if label == 1 and word in tokenize(content))
        num_spam = len(self.spam_mail_list)
        return num_spam_with_word / num_spam


    """
    Task 4. P(word_i|S=ham)
    Return the conditional probability (0.0~1.0) that a ham email has the given word
    """
    def ham_cond_prob(self, word):
        num_ham_with_word = sum(1 for content, label in self.train_dataset if label == 0 and word in tokenize(content))
        num_ham = len(self.ham_mail_list)
        return num_ham_with_word / num_ham

    """
    2-(g) strategy to improve the accuracy 위의 코드를 아래 코드처럼 바꾼다.
    
    def spam_cond_prob(self, word, alpha=1.0):
        num_spam_with_word = sum(1 for content, label in self.train_dataset if label == 1 and word in tokenize(content))
        num_spam = len(self.spam_mail_list)
        return (num_spam_with_word + alpha) / (num_spam + alpha * len(self.all_words_list))

    def ham_cond_prob(self, word, alpha=1.0):
        num_ham_with_word = sum(1 for content, label in self.train_dataset if label == 0 and word in tokenize(content))
        num_ham = len(self.ham_mail_list)
        return (num_ham_with_word + alpha) / (num_ham + alpha * len(self.all_words_list))

    """




    """
    You can add other member functions and variables if needed
    """
############################################################################################################


############################################################################################################
# Part 3 - Train / inference procedure
# You must include your implementation in the inference() function
"""
Train the model
- Note: you don't need to touch this function
  since you can implement all of your training strategies in the NBModel class
"""
def train(train_dataset):
    return NBModel(train_dataset)

"""
Inference with the model
- Note: you need to fill the ?? parts with the functions that you implement earlier, e.g., model.spam_cond_prob(word), etc
"""
def inference(model, test_dataset):
    n_correct = 0
    for content, is_spam in test_dataset:
        words_set = tokenize(content)  # The set of words in an tested email (i.e., inference)

        ########################################################
        spam_prob = model.spam_prob()
        ham_prob = 1 - spam_prob  # P(ham) = 1 - P(spam)

        for word in words_set:
            if model.word_exists(word):
                spam_prob *= model.spam_cond_prob(word)
                ham_prob *= model.ham_cond_prob(word)

        ########################################################

        # If the probability of spam is higher than that of ham, then we predict it as a spam
        is_spam_prediction = spam_prob >= ham_prob
        if is_spam_prediction == is_spam:
            n_correct += 1

    n_samples = len(test_dataset)
    accuracy = n_correct / n_samples
    print("Accuracy {} ({} / {})\n".format(accuracy, n_correct, n_samples))
############################################################################################################


############################################################################################################
# Part 4 - Scikit-learn
# You need to modify this to complete your report
def run_sklearn(train_dataset, test_dataset):
    # Prepare the dataset and vectorize (make a matrix that counts the number of the appereances for each word)
    X_train_text, y_train = zip(*train_dataset)
    X_test_text, y_test = zip(*test_dataset)
    n_train = len(X_train_text)

    vectorizer = CountVectorizer()
    X = vectorizer.fit_transform(X_train_text + X_test_text)
    X_train = X[:n_train]
    X_test = X[n_train:]

    # Model Training
    model = MultinomialNB()
    # 2-(f) - model = BernoulliNB
    # 2-(f) - model = ComplementNB 로 각각 교체해서 코드를 돌림.
    model.fit(X_train, y_train)
    
    # Inference
    preds_test = model.predict(X_test)

    n_correct = sum(1 for pred, real in zip(preds_test, y_test) if pred == real)
    n_samples = len(test_dataset)
    accuracy = n_correct / n_samples
    print("Accuracy (scikit-learn) {} ({} / {})\n".format(accuracy, n_correct, n_samples))


############################################################################################################


def main():
    train_dataset, test_dataset = read_csv()

    # Run your implementation
    model = train(train_dataset)
    inference(model, test_dataset)

    # Run the sklearn implementation
    run_sklearn(train_dataset, test_dataset)
        

if __name__ == "__main__":
    main()

###########################################################################################################
# 2-(g)는 def spam_cond_prob와 def ham_cond_prob를 변화시킨다. 아래는 변화 코드이다.
"""

def spam_cond_prob(self, word, alpha=1.0):
    num_spam_with_word = sum(1 for content, label in self.train_dataset if label == 1 and word in tokenize(content))
    num_spam = len(self.spam_mail_list)
    return (num_spam_with_word + alpha) / (num_spam + alpha * len(self.all_words_list))

def ham_cond_prob(self, word, alpha=1.0):
    num_ham_with_word = sum(1 for content, label in self.train_dataset if label == 0 and word in tokenize(content))
    num_ham = len(self.ham_mail_list)
    return (num_ham_with_word + alpha) / (num_ham + alpha * len(self.all_words_list))

"""

"""
2.(e)
먼저, spam_prob()를 구현하여서, 전체 중에서 spam이 얼마나 있는지를 계산한다. 이는 전체에서 spam이 나올 확률이다. 이 implementation에서는 word_exists()를 사용하여서, 각각 스팸과 햄 메일에 특정 단어들이 들어가 있는 조건부 확률을 계산한다. 모델은 위의 가능성, 즉 확률에 기반하여서 이메일이 스팸인지 햄인지 분류한다.
결과값은 다음과 같다.
Accuracy 0.9491626794258373 (1587 / 1672)
Accuracy (scikit-learn) 0.9796650717703349 (1638 / 1672)
2.(f)
각기 실행을 시켜 보았다. 결과값은 다음과 같다.
Accuracy 0.9491626794258373 (1587 / 1672) ( 셋 다 동일)

MultinomialNB - Accuracy (scikit-learn) 0.9796650717703349 (1638 / 1672)
BernoulliNB - Accuracy (scikit-learn) 0.9766746411483254 (1633 / 1672)
ComplementNB - Accuracy (scikit-learn) 0.9617224880382775 (1608 / 1672)
결론적으로, 대부분 비슷하였지만, ComplementNB가 다른 방식보다 정확도가 떨어졌고, MultinomialNB가 우리가 사용하는 코드에서는 가장 정확도가 높은 것으로 드러났다.

2.(g)
정확도를 향상시키는 방법 중, Laplace smoothing 기법을 사용하였다. 이는 매개변수 alpha를 도입하여서, train set에 없는 단어에 대한 확률을 제공하고, 0으로 나누어지는 것을 피할 수 있다.
바꾼 코드는 다음과 같다.
def spam_cond_prob(self, word, alpha=1.0):
    num_spam_with_word = sum(1 for content, label in self.train_dataset if label == 1 and word in tokenize(content))
    num_spam = len(self.spam_mail_list)
    return (num_spam_with_word + alpha) / (num_spam + alpha * len(self.all_words_list))

def ham_cond_prob(self, word, alpha=1.0):
    num_ham_with_word = sum(1 for content, label in self.train_dataset if label == 0 and word in tokenize(content))
    num_ham = len(self.ham_mail_list)
    return (num_ham_with_word + alpha) / (num_ham + alpha * len(self.all_words_list))
이것의 결과값은 다음과 같다.
Accuracy 0.9671052631578947 (1617 / 1672)
Accuracy (scikit-learn) 0.9796650717703349 (1638 / 1672)
즉, 이 smoothing 기법을 이용하였더니, 

Accuracy 0.9491626794258373 (1587 / 1672)에서
Accuracy 0.9671052631578947 (1617 / 1672)으로 증가된 것을 알 수 있다.(약 0.02 증가 )



"""