#include <array>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>

int main()
{
	// Chargement des textures
	sf::Texture crosshair_outline_small;
	crosshair_outline_small.loadFromFile("Assets\\PNG\\HUD\\crosshair_outline_small.png");

	sf::Texture crosshair_red_large;
	crosshair_red_large.loadFromFile("Assets\\PNG\\HUD\\crosshair_red_large.png");

	sf::Texture background;
	background.loadFromFile("Assets\\PNG\\Stall\\bg_wood.png");

	sf::Texture curtain;
	curtain.loadFromFile("Assets\\PNG\\Stall\\curtain.png");

	sf::Texture decoration;
	decoration.loadFromFile("Assets\\PNG\\Stall\\curtain_straight.png");

	sf::Texture game_over_texture;
	game_over_texture.loadFromFile("Assets\\PNG\\HUD\\text_gameover.png");

	sf::Texture times_up_texture;
	times_up_texture.loadFromFile("Assets\\PNG\\HUD\\text_timeup.png");

	sf::Texture icon_duck;
	icon_duck.loadFromFile("Assets\\PNG\\HUD\\icon_duck.png");

	sf::Texture icon_target;
	icon_target.loadFromFile("Assets\\PNG\\HUD\\icon_target.png");

	std::array<sf::Texture, 3> targets_textures;
	targets_textures[0].loadFromFile("Assets\\PNG\\Objects\\duck_outline_target_brown.png");
	targets_textures[1].loadFromFile("Assets\\PNG\\Objects\\duck_outline_target_white.png");
	targets_textures[2].loadFromFile("Assets\\PNG\\Objects\\duck_outline_target_yellow.png");

	// Declaration des sprites
	// Sprite de fin de partie : defaite
	sf::Sprite game_over;
	game_over.setTexture(game_over_texture);
	game_over.setOrigin({ 349 / 2, 72 / 2 });
	game_over.setPosition({ 640, 360 });
	game_over.setScale(2.0f, 2.0f);

	// Sprite de fin de partie : temps ecoule = victoire
	sf::Sprite times_up;
	times_up.setTexture(times_up_texture);
	times_up.setOrigin({ 349 / 2, 72 / 2 });
	times_up.setPosition({ 640, 360 });
	times_up.setScale(2.0f, 2.0f);

	// Sprite du viseur
	sf::Sprite crosshair;
	crosshair.setTexture(crosshair_outline_small);
	crosshair.setOrigin({ 23, 23 });

	// Tableau des cibles
	std::vector<sf::Sprite> targets;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");
	window.setMouseCursorVisible(false);

	// Outils de gestion du temps
	sf::Clock clock;
	double time_elapsed = 0.0;
	double time_limit = 2.0;
	int target_missed = 0;
	int score = 0;

	// Random generator with random engines from standard library
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> x_gen(100, 1180);
	std::uniform_real_distribution<float> y_gen(100, 720);
	std::uniform_int_distribution<int> target_gen(0, targets_textures.size() - 1);

	// Main loop
	while (window.isOpen())
	{
		// Gestion des events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Game logic --------------------------------------------
		time_elapsed += clock.restart().asSeconds();
		if (target_missed < 5 && !(time_limit <= 0.1))
		{
			// Quand le temps est ecoule, on ajoute une cible
			if (time_elapsed >= time_limit)
			{
				time_elapsed = 0.0;
				// Le temps limite diminue de 0.05 secondes a chaque nouvelle cible => le jeu devient plus difficile
				time_limit -= 0.05;

				targets.emplace_back();
				sf::Sprite& target = targets.back();
				target.setTexture(targets_textures[target_gen(gen)]);
				target.setOrigin({ 114, 109 });
				target.setPosition({ x_gen(gen), y_gen(gen) });

			}

			// Position du viseur sur la souris --------------------------------------------
			sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
			crosshair.setPosition({ (float)mouse_pos.x, (float)mouse_pos.y });

			// Verification de chacune des cibles --------------------------------------------
			for (auto it = targets.begin(); it != targets.end(); )
			{
				// La cible retrecit a chaque frame
				it->setScale(it->getScale() * 0.9995f);
				// Si la cible est trop petite, elle est detruite
				// 5 cibles qui se detruisent = game over
				if (it->getScale().x < 0.6f)
				{
					// Apres une suppression, on recuppere l'iterateur suivant
					// => pas besoin de faire it++
					it = targets.erase(it);
					target_missed++;
					// On passe directement a l'iteration suivante
					continue;

				}

				// Si le viseur collisionne avec une cible, celle ci est detruite
				if (it->getGlobalBounds().intersects(crosshair.getGlobalBounds()))
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
					{
						// Apres une suppression, on recupere l'iterateur suivant
						// => pas besoin de faire it++
						it = targets.erase(it);
						score++;
						// On passe directement a l'iteration suivante
						continue;
					}
				}

				// Pas de suppression, alors on incremente l'iterateur
				++it;
			}
		}

		// Rendering --------------------------------------------
		window.clear();

		// Background --------------------------------------------
		sf::RectangleShape background_rect;
		background_rect.setSize({ 46, 46 });
		background_rect.setTexture(&background);

		for (int i = 0; i < 1280; i += 46)
		{
			for (int j = 0; j < 720; j += 46)
			{
				background_rect.setPosition({ (float)i, (float)j });
				window.draw(background_rect);
			}
		}

		// Draw targets --------------------------------------------
		// On affiche soit les cibles a tirer, soit le message de fin de partie (Perdu ou Gagne)
		std::cout << "Targets missed : " << target_missed << " - Time limit : " << time_limit << '\n';
		if (target_missed >= 5)
		{
			window.draw(game_over);
		}
		else if (time_limit <= 0.1)
		{
			window.draw(times_up);
		}
		else
		{
			for (auto& target : targets)
			{
				window.draw(target);
			}
		}

		// Decorations --------------------------------------------
		// Rideaux
		sf::RectangleShape curtain_rect;
		curtain_rect.setTexture(&curtain);
		curtain_rect.setPosition({ -20, 0 });
		curtain_rect.setSize({ 150, 800 });
		window.draw(curtain_rect);

		curtain_rect.setScale(-1, 1);
		curtain_rect.setPosition({ 1300, 0 });
		window.draw(curtain_rect);

		// Rideau du haut
		sf::RectangleShape decoration_rect;
		decoration_rect.setSize({ 256, 80 });

		for (int i = 0; i < 1280; i += 256)
		{
			decoration_rect.setPosition({ (float)i, 0 });
			decoration_rect.setTexture(&decoration);
			window.draw(decoration_rect);
		}

		// Draw score --------------------------------------------
		sf::Sprite icon;
		icon.setOrigin({ 23, 23 });

		// Les canards affichent le score
		icon.setTexture(icon_duck);
		for (int i = 0; i < score; i++)
		{
			icon.setPosition({ 50.f + (i % 12) * 50.f, (1 + i / 12) * 50.0f });
			window.draw(icon);
		}

		// Les cibles affichent le nombre de cibles manquees (condition de defaite)
		icon.setTexture(icon_target);
		for (int i = 0; i < target_missed; i++)
		{
			icon.setPosition({ 1280 - (float)(50 + i * 50), 50 });
			window.draw(icon);
		}

		// Dessin du viseur --------------------------------------------
		window.draw(crosshair);
		window.display();

	}

	return 0;
}
