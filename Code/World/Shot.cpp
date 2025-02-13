#include "Shot.h"

#include "Enemy.h"
#include "Meteor.h"
#include "Player.h"
#include "Saucer.h"
#include "World.h"
#include "../Physics/Collision.h"
#include "../Tools/Random.h"
#include "../Tools/Math.h"

#pragma region class Shot

Shot::Shot(Configuration::Texture textureID, World& world, float moveSpeed)
	: Entity(textureID, world)
{
	this->moveSpeed = moveSpeed;

	// Random angle from 0 to 360 degrees (in radiands):
	float angleInRadians = Random::GenerateFloat(0.0f, 2.0f * Math::PI);

	// Convert the angle from polar coordinates to Cartesian coordinates, and
	// set the result to moveDirection:
	moveDirection = sf::Vector2f(std::cos(angleInRadians), std::sin(angleInRadians));
}

void Shot::Update(float deltaTime)
{
	sprite.move(moveDirection * moveSpeed * deltaTime);

	duration -= deltaTime;
	if (duration < 0)
		isAlive = false;
}

#pragma endregion

#pragma region class PlayerShot

PlayerShot::PlayerShot(Player& whoShoots)
	: Shot(Configuration::Texture::PlayerShot, whoShoots.world, 1000.0f)
{
	duration = 5.0f;

	// Convert degrees to radians and subtract 90 degrees (in radians):
	float angleInRadians = whoShoots.sprite.getRotation() / 180 * Math::PI - Math::PI / 2;

	// Convert the angle from polar coordinates to Cartesian coordinates, and
	// set the result to moveDirection:
	moveDirection = sf::Vector2f(std::cos(angleInRadians), std::sin(angleInRadians));

	SetPosition(whoShoots.GetPosition());
	sprite.setRotation(whoShoots.sprite.getRotation());

	world.Add(Configuration::Sound::PlayerLaserShot);
}

bool PlayerShot::IsCollideWith(const Entity& other) const
{
	if (dynamic_cast<const Enemy*>(&other) != nullptr)
		return Collision::CheckCircleCollision(sprite, other.sprite);
	else
		return false;
}

#pragma endregion

#pragma region SaucerShot

SaucerShot::SaucerShot(SaucerShooter& saucer)
	: Shot(Configuration::Texture::EnemySaucerShot, saucer.GetWorld(), 750.0f)
{
	duration = 5.0f;

	sf::Vector2f distanceToPlayer = Configuration::player->GetPosition() - saucer.GetPosition();

	// Calculate a random adjustment to the angle (in radians) to make the shot less accurate. The greater score user
	// has - the less accuracy will be for the shot:
	float accuracyLostAngleInRadians = Random::GenerateFloat(-1.0f, 1.0f) * Math::PI / ((200 + Configuration::GetScore()) / 100.0f);

	// Calculate the angle from the x-axis to the vector pointing towards the player:
	float angleInRadians = std::atan2(distanceToPlayer.y, distanceToPlayer.x) + accuracyLostAngleInRadians;
	float angleInDegrees = angleInRadians * 180 / Math::PI;

	// Convert the angle from polar coordinates to Cartesian coordinates, and
	// set the result to moveDirection:
	moveDirection = sf::Vector2f(std::cos(angleInRadians), std::sin(angleInRadians));

	SetPosition(saucer.GetPosition());
	sprite.setRotation(angleInDegrees + 90.0f);
	world.Add(Configuration::Sound::EnemyLaserShot);
}

bool SaucerShot::IsCollideWith(const Entity& other) const
{
	if (dynamic_cast<const Player*>(&other) != nullptr || dynamic_cast<const Meteor*>(&other) != nullptr)
		return Collision::CheckCircleCollision(sprite, other.GetSprite());
	else
		return false;
}

#pragma endregion