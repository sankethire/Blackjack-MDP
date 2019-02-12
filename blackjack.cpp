#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector<vector<float> > dealer_card(float p_face){
	float p = (1.0-p_face)/9.0;
	vector<vector<float> > dealcar;
	vector<float> element;
	for(int i=0;i<=32;i++)element.push_back(0);
	for(int i=0;i<7;i++)dealcar.push_back(element);
	// vector<vector<float> > dealcar(7, vector<float> (33,0));

	for(int i=0;i<5;i++){
		for(int j=17;j<=21;j++){
			if(j==17+i)dealcar[i][j] = 1;
			else dealcar[i][j] = 0;
		}
		for(int j=28;j<=32;j++){
			if(j==28+i)dealcar[i][j] = 1;
			else dealcar[i][j] = 0;
		}
	}

// for hard cards, from 11 to 16
	for(int i=0;i<5;i++){
		for(int j=16;j>=11;j--){
			float score = 0;
			for(int k=1;k<=9;k++){
				if(j+k<=21){
					score += dealcar[i][j+k];
				}
			}
			if(j>11)dealcar[i][j] = score * p;
			else if(j==11)dealcar[i][j] = score*p + p_face*dealcar[i][j+10];
		}
	}
// for soft hand cards, from 11 to 16
// j = 22 means soft 11 and j=27 means soft 16
	for(int i=0;i<5;i++){
		for(int j=16;j>11;j--){
			float score1 = 0;
			for(int k=1;k<=21-j;k++){
				score1 += dealcar[i][j+11+k];
			}
			score1 *= p;
			float score2 = 0;
			for(int k=22-j;k<=9;k++){
				score2 += dealcar[i][j+k-10];
			}
			score2 *= p;
			dealcar[i][j+11] = score1 + score2 + p_face*dealcar[i][j];
		}
	}
// for soft 11
	for(int i=0;i<5;i++){
		float score1=0;
		for(int k=1;k<=9;k++){
			score1 += dealcar[i][22+k];
		}
		dealcar[i][22] = p*score1;
	}
// for hard 2 to 10
	for(int i=0;i<5;i++){
		for(int j=10;j>=2;j--){
			float score1 = 0;
			for(int k=2;k<=9;k++)score1 += dealcar[i][j+k];
			score1 *= p;
			dealcar[i][j] = score1 + p_face*dealcar[i][j+10] + p*dealcar[i][j+22];
		}
	}

	vector<vector<float> > cards;
	vector<float> els;
	for(int i=0;i<7;i++)els.push_back(0);
	for(int i=1;i<=10;i++)cards.push_back(els);

	for(int i=0;i<9;i++){
		for(int j=0;j<5;j++){
			cards[i+1][j] = dealcar[j][i+2];
		}
	}
	for(int j=0;j<5;j++)cards[0][j] = dealcar[j][22];
	cards[9][4] -= p;
	cards[9][5] = p;
	cards[0][5] = p_face;

	for(int i=0;i<10;i++){
		cards[i][6] = 1;
		for(int j=0;j<6;j++)cards[i][6] -= cards[i][j];
	}	
	return cards;
}
// cards is a two dimension-vector where in 1st row the dealer face-up card is Ace and subsequently cards are 2 to 10
// column wise - 17 18 19 20 21 BlackJack Bust

int main(int argc, char *argv[]){
	string p_face_string = argv[1];
	float p_face = stof(p_face_string);
	float p = (1-p_face)/9.0;
	vector<vector<float> > d = dealer_card(p_face);

	vector<vector<float> > stand(22, vector<float> (10,0));

	for(int i=4;i<=21;i++){
		for(int j=0;j<=9;j++){
			if(i<17)stand[i][j] = 2*d[j][6] - 1;
			else{
				float s = 0;
				for(int k=0;k<5;k++){
					if(i>k+17) s += d[j][k];
					else if(i<k+17) s -= d[j][k];
				}
				stand[i][j] = s+d[j][6]-d[j][5];
			}
		}
	}

	vector<vector<float> > hard_hand(22,vector<float> (10,0));

	for(int i=21;i>=12;i--){
		for(int j=0;j<=9;j++){
			float score=0;
			for(int k=1;k<=21-i;k++)score += hard_hand[i+k][j];
			score *= p;
			score = score - p*(i-12) - p_face;
			hard_hand[i][j] = max(score,stand[i][j]);
		}
	}
	
	for(int j=0;j<=9;j++){
		float score=0;
		for(int k=1;k<=9;k++)score += hard_hand[11+k][j];
		score *= p;
		score += p_face*hard_hand[21][j];
		hard_hand[11][j] = max(score, stand[11][j]);
	}

	vector<vector<float> > soft_hand(12,vector<float> (10,0));

// for soft hand valued from 12 to 21
	for(int i=21;i>=12;i--){
		for(int j=0;j<=9;j++){
			float score=0;
			for(int k=1;k<=21-i;k++)score += soft_hand[i-10+k][j];
			score *= p;
			float score2=0;
			for(int k=22-i;k<=9;k++)score2 += hard_hand[i-10+k][j];
			score2 *= p;
			score = score+score2+p_face*hard_hand[i][j];
			soft_hand[i-10][j] = max(score, stand[i][j]);
		}
	}
	for(int i=10;i>=4;i--){
		for(int j=0;j<=9;j++){
			float score=0;
			for(int k=2;k<=9;k++)score += hard_hand[i+k][j];
			score = score*p + p*soft_hand[i+11-10][j] + p_face*hard_hand[i+10][j];
			hard_hand[i][j] = max(score, stand[i][j]);
		}
	}

	vector<vector<float> > double_hard(22,vector<float> (10,0));

	for(int i=20;i>=12;i--){
		for(int j=0;j<=9;j++){
			float score=0;
			for(int k=1;k<=21-i;k++)score += 2*p*stand[i+k][j];
			double_hard[i][j] = score-2*p*(i-12) - 2*p_face;
		}
	}
	
	for(int j=0;j<=9;j++){
		float score=0;
		for(int k=1;k<=9;k++)score += 2*p*stand[11+k][j];
		double_hard[11][j] = score + 2*p_face*stand[21][j];
	}

	for(int i=10;i>=4;i--){
		for(int j=0;j<=9;j++){
			float score=0;
			for(int k=2;k<=9;k++)score += stand[i+k][j];
			double_hard[i][j] = 2*p*score + 2*p_face*stand[i+10][j] + 2*p*stand[i+11][j];
		}
	}

	vector<vector<float> > double_soft(11,vector<float> (10,0));

	for(int i=20;i>=12;i--){
		for(int j=0;j<=9;j++){
			float score1 = 0;
			float score2 = 0;
			for(int k=1;k<=21-i;k++)score1 += stand[i+k][j];
			for(int k=22-i;k<=9;k++)score2 += stand[i+k-10][j];
			double_soft[i-10][j] = 2*p*(score1+score2) + 2*p_face*stand[i][j];
		}
	}

	vector<vector<pair<float, char> > > double_hit_hard;
	pair<float, char> x;
	x.first = -10;
	x.second = 'A';
	vector<pair<float, char> > el;
	for(int j=0;j<10;j++)el.push_back(x);
	for(int i=0;i<4;i++)double_hit_hard.push_back(el);
	for(int i=4;i<22;i++){
		vector<pair<float, char> > dhh_row;
		for(int j=0;j<10;j++){
			pair<float, char> value;
			if(hard_hand[i][j] > double_hard[i][j]){
				value.first = hard_hand[i][j];
				if(value.first == stand[i][j])value.second = 'S';
				else value.second = 'H';
			}
			else{
				value.first = double_hard[i][j];
				value.second = 'D';
			}
			dhh_row.push_back(value);
		}
		double_hit_hard.push_back(dhh_row);
	}

	vector<vector<pair<float, char> > > double_hit_soft;
	for(int i=0;i<2;i++)double_hit_soft.push_back(el);
	for(int i=2;i<11;i++){
		vector<pair<float, char> > dhh_row;
		for(int j=0;j<10;j++){
			pair<float, char> value;
			if(soft_hand[i][j] > double_soft[i][j]){
				value.first = soft_hand[i][j];
				if(value.first == stand[i+10][j])value.second = 'S';
				else value.second = 'H';
			}
			else{
				value.first = double_soft[i][j];
				value.second = 'D';
			}
			dhh_row.push_back(value);
		}
		double_hit_soft.push_back(dhh_row);
	}

	vector<vector<pair<float, char> > > spilt_card;

// for the pair of faces(10)
	vector<pair<float, char> > ai;
	for(int j=0;j<10;j++){
		float score = 0;
		pair<float, char> xy;
		for(int k=2;k<10;k++)score += double_hit_hard[10+k][j].first;
		score = 2*p*score + 3*p*(1-d[j][5]);

		score /= (1-2*p_face);
		if(score>double_hit_hard[20][j].first){
			xy.first = score;
			xy.second = 'P';
		}
		else xy = double_hit_hard[20][j];
		ai.push_back(xy);
	}
	spilt_card.push_back(ai);
// for the pair of aces(11)	
	ai.clear();
	for(int j=0;j<10;j++){
		pair<float, char> xy;
		float score = 0;
		for(int k=1;k<10;k++){
			score += stand[k+11][j];
		}		
		score = 2*p*score;
		score = score + 2*p_face*stand[21][j];
		if(score > double_hit_soft[2][j].first){
			xy.first = score;
			xy.second = 'P';
		}
		if(score < double_hit_soft[2][j].first){
			 xy.first = double_hit_soft[2][j].first;
			 xy.second = double_hit_soft[2][j].second;
		}
		ai.push_back(xy);
	}
	
	spilt_card.push_back(ai);

	for(int i=2;i<10;i++){
		vector<pair<float, char> > ai;
		for(int j=0;j<10;j++){
			pair<float, char> xy;
			float score = 0;
			for(int k=2;k<10;k++){
				if(k!=i)score += double_hit_hard[i+k][j].first;
			}
			score = 2*p*score + 2*p*double_hit_soft[i+1][j].first + 2*p_face*double_hit_hard[i+10][j].first;
			score /= (1-2*p);
			if(score>double_hit_hard[2*i][j].first){
				xy.first = score;
				xy.second = 'P';
			}
			else xy = double_hit_hard[2*i][j];
			ai.push_back(xy);
		}
		spilt_card.push_back(ai);
	}

	for(int i=5;i<20;i++){
		cout <<  i << '\t';
		for(int j=1;j<10;j++)cout << double_hit_hard[i][j].second << " ";
		cout << double_hit_hard[i][0].second << "\n";
	}
	for(int i=3;i<11;i++){
	cout << "A" << i-1 << '\t';
	for(int j=1;j<10;j++)cout << double_hit_soft[i][j].second << ' ';
	cout << double_hit_soft[i][0].second << "\n";
	}

	for(int i=2;i<10;i++){
		cout << i << i << '\t';
		for(int j=1;j<10;j++)cout << spilt_card[i][j].second << " ";
		cout << spilt_card[i][0].second << "\n";
	}
	cout << 1010 << '\t';
	for(int j=1;j<10;j++)cout << spilt_card[0][j].second << " ";
	cout << spilt_card[0][0].second << "\n";

	cout << "AA" << '\t';
	for(int j=1;j<10;j++)cout << spilt_card[1][j].second << " ";
	cout << spilt_card[1][0].second ;

	return 0;
}