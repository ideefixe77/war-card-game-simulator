#include <stdio.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

struct put_s
{
	int card_1;
	int card_2;
};

enum sort_type
{
	SORT_RANDOM,
	SORT_NORMAL,
	SORT_REVERSE
};

enum strategy_type
{
	RANDOM_STRATEGY = 0,
	PLAYER_1_FIRST = 1,
	PLAYER_2_FIRST = 2,
	STRONGER_FIRST = 3,
	WEAKER_FIRST = 4,
	BEST_STRATEGY = 5
};

struct strategy_s
{
	strategy_type player_1;
	strategy_type player_2;
};

void prepare_cards(vector<int> &deck)
{
	for (int n = 0; n < 4; n++)
		for (int i = 2; i <= 14; i++)
			deck.push_back(i);
}

void shuffle_cards(vector<int> &deck)
{
	int cards = deck.size();

	for (int i = 0; i < cards; i++)
	{
		int j = rand() % cards;
		int tmp = deck[i];
		deck[i] = deck[j];
		deck[j] = tmp;
	}
}

void distribute_cards(list<int> &player_1, list<int> &player_2, vector<int> &deck, int cards = 52)
{
	for (int i = 0; i < cards;)
	{
		player_1.push_back(deck[i++]);
		player_2.push_back(deck[i++]);
	}
}

void reverse_cards(vector<int> &decs)
{
	for (unsigned int i = 0, j = decs.size() - 1; i < j; i++, j--)
	{
		int tmp = decs[i];
		decs[i] = decs[j];
		decs[j] = tmp;
	}
}

int count_cards(list<int> &player, int card)
{
	int count = 0;

	for each(int c in player)
	{
		if (c == card)
			count++;
	}

	return count;
}

int players_power(list<int> &player)
{
	int power = 0;

	for each(int c in player)
	{
		power += c;
	}

	return power;
}

bool put_cards(list<put_s> &pool, list<int> &player_1, list<int> &player_2)
{
	put_s put = { 0, 0 };
	bool ok = true;
	
	if (player_1.size())
	{
		put.card_1 = player_1.front();
		player_1.pop_front();
	}
	else
		ok = false;

	if (player_2.size())
	{
		put.card_2 = player_2.front();
		player_2.pop_front();
	} else
		ok = false;

	pool.push_front(put);
	return ok;
}

void take_pool_player_1_first(list<int> &winner, list<put_s> &pool)
{
	for each (put_s p in pool)
	{
		if (p.card_1 != 0)
			winner.push_back(p.card_1);
		if (p.card_2 != 0)
			winner.push_back(p.card_2);
	}
}

void take_pool_player_2_first(list<int> &winner, list<put_s> &pool)
{
	for each (put_s p in pool)
	{
		if (p.card_2 != 0)
			winner.push_back(p.card_2);
		if (p.card_1 != 0)
			winner.push_back(p.card_1);
	}
}

void take_pool_sorted(list<int> &winner, list<put_s> &pool, sort_type sort)
{
	vector<int> tmp;

	for each (put_s p in pool)
	{
		if (p.card_1 != 0)
			tmp.push_back(p.card_2);
		if (p.card_2 != 0)
			tmp.push_back(p.card_1);
	}
	if (sort == SORT_NORMAL || sort == SORT_REVERSE)
		std::sort(tmp.begin(), tmp.end());
	if (sort == SORT_REVERSE)
		reverse_cards(tmp);
	if (sort == SORT_RANDOM)
		shuffle_cards(tmp);
	for each (int card in tmp)
	{
		winner.push_back(card);
	}
	tmp.clear();
}

void take_pool(list<int> &winner, list<put_s> &pool, strategy_type strategy)
{
	switch (strategy)
	{
	case RANDOM_STRATEGY:
		take_pool_sorted(winner, pool, SORT_RANDOM);
		break;
	case PLAYER_1_FIRST:
		take_pool_player_1_first(winner, pool);
		break;
	case PLAYER_2_FIRST:
		take_pool_player_2_first(winner, pool);
		break;
	case STRONGER_FIRST:
		take_pool_sorted(winner, pool, SORT_REVERSE);
		break;
	case WEAKER_FIRST:
		take_pool_sorted(winner, pool, SORT_NORMAL);
		break;
	case BEST_STRATEGY:
		if (pool.size() == 1)
			take_pool_sorted(winner, pool, SORT_REVERSE);
		else
			take_pool_sorted(winner, pool, SORT_REVERSE);
		break;
	}
}

void finish_game(list<int> &player_1, list<int> &player_2, list<put_s> &pool)
{
	if (player_1.size())
		take_pool_sorted(player_1, pool, SORT_RANDOM);
	else
		take_pool_sorted(player_2, pool, SORT_RANDOM);
}

int match(list<int> &player_1, list<int> &player_2, strategy_s strategy)
{
	int wars = 0;
	list<put_s> pool;
	list<put_s>::iterator pi;
	
	put_cards(pool, player_1, player_2);
	pi = pool.begin();

	while (pi->card_1 == pi->card_2)
	{
		wars++;
		if (put_cards(pool, player_1, player_2) == false // hidden pair
			|| put_cards(pool, player_1, player_2) == false)
		{
			finish_game(player_1, player_2, pool);
			pool.clear();
			return wars;
		}
		pi = pool.begin();
	}

	if (pi->card_1 > pi->card_2)
		take_pool(player_1, pool, strategy.player_1);
	else
		take_pool(player_2, pool, strategy.player_2);

	pool.clear();
	return wars;
}

unsigned long long max_rounds = 0;

int game(int decs, strategy_s strategy)
{
	list<int> player_1, player_2;
	unsigned long long rounds = 0, wars = 0;
	unsigned long long war[10] = { 0 };
	int max_wars = 0, no_war = 0, max_no_war = 0;
	time_t start = time(0);
	int ret = 0;

	for (int t = 0; t < decs; t++)
	{
		vector<int> deck;
		prepare_cards(deck);
		shuffle_cards(deck);
		distribute_cards(player_2, player_1, deck);
		deck.clear();
	}

	printf("A: A=%d, K=%d, Q=%d, power=%d\n", count_cards(player_1, 14), count_cards(player_1, 13), count_cards(player_1, 12), players_power(player_1));
	printf("B: A=%d, K=%d, Q=%d, power=%d\n", count_cards(player_2, 14), count_cards(player_2, 13), count_cards(player_2, 12), players_power(player_2));

	while (player_1.size() && player_2.size())
	{
		int w = match(player_1, player_2, strategy);
		wars += w;
		if (w > max_wars)
			max_wars = w;
		if (w < sizeof(war) / sizeof(unsigned long long))
			war[w]++;
		// Check no results
		if (w)
		{
			if (no_war > max_no_war)
				max_no_war = no_war;
			no_war = 0;
		}
		else
			if (++no_war > 520)
				break;
		rounds++;
		if (rounds > 52 * decs * 1000)
			break;
	}

	if (rounds < 52 * decs * 1000 && rounds > max_rounds)
		max_rounds = rounds;

	if (player_1.size() && player_2.size())
		printf("Draw, time: %llds, rounds: %llu, wars: %llu (max: %d), max_no_war: %d\n", time(0) - start, rounds, wars, max_wars, max_no_war);
	else
		printf("Won: %s (%d, %d), time: %llds, rounds: %llu, wars: %llu (max: %d), max_no_war: %d\n", (player_1.size()) ? "A" : "B", player_1.size(), player_2.size(), time(0) - start, rounds, wars, max_wars, max_no_war);
	for (int w = 0; w < sizeof(war) / sizeof(unsigned long long); w++)
	{
		if (war[w])
			printf("%d=%llu ", w, war[w]);
	}
	printf("\n");

	if (player_1.size() > 0 && player_2.size() == 0)
		ret = 1;
	else 
	if (player_2.size() > 0 && player_1.size() == 0)
		ret = 2;
	
	player_1.clear();
	player_2.clear();

	return ret;
}

int main(int argc, char* argv[])
{
	int desc = 1, games = 100;
	int won[3] = { 0 };
	strategy_s strategy = { RANDOM_STRATEGY, RANDOM_STRATEGY };

	if (argc > 1)
		desc = atoi(argv[1]);
	if (argc > 2)
		games = atoi(argv[2]);
	if (argc > 3)
		strategy.player_1 = (strategy_type)atoi(argv[3]);
	if (argc > 4)
		strategy.player_2 = (strategy_type)atoi(argv[4]);

	srand((unsigned int)time(0));

	while (games--)
	{
		printf("Game=%d\n", games);
		int w = game(desc, strategy);
		++won[w];
	}

	printf("Draws=%d, A=%d, B=%d, max_rounds=%llu\n", won[0], won[1], won[2], max_rounds);
	
	return 0;
}

