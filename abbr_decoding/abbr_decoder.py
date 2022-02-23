import joblib
import numpy as np
import pandas as pd

from normalizer import normalize_sentences


abbrs = set(pd.read_csv('../data/abbr_term.csv', sep='\t')['abbreviation'].str.lower())


class AbbreviationDecoder:
    def __init__(self):
        self.threshold = 0.5
        self.context_vectorizer = joblib.load('context_vectorizer.joblib')
        self.char_vectorizer = joblib.load('char_vectorizer.joblib')
        self.model = joblib.load('decoder_model.joblib')
    
    def _normalize_sentences(self, sentences):
        sentences = normalize_sentences(
            sentences, 
            except_words=abbrs,
        )
        clean_sentences = [" ".join([" ".join(word) for word in text]) for text in sentences]
        
        return clean_sentences
    
    def _get_features(self, context, abbreviation):
        if isinstance(context, str):
            context = [context]
            abbreviation = [abbreviation]
            
        context = self._normalize_sentences(context)
            
        context_features = self.context_vectorizer.transform(context).toarray()
        abbr_features = self.char_vectorizer.transform(abbreviation).toarray()

        features = np.hstack((context_features, abbr_features))

        return features

    def predict(self, abbreviation, context):
        features = self._get_features(context, abbreviation)
        prob = np.max(self.model.predict_proba(features))
        
        if prob > self.threshold:
            return self.model.predict(features)
        
        return [abbreviation]