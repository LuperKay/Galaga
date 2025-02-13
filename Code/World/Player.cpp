#include "Player.h"

#include "Shot.h"
#include "World.h"
#include "../Configuration.h"
#include "../Physics/Collision.h"
#include "../Tools/Random.h"
#include "../Tools/Math.h"

Player::Player(World& world)
	: Entity(Configuration::Texture::PlayerShip, world)
	, TargetForActions(Configuration::playerActions)
{
	moveSpeed = 500.0f;

	Bind(
		Configuration::PlayerAction::Up,
		[this](const sf::Event&) { isMoving = true; }
	);

	Bind(
		Configuration::PlayerAction::Left,
		[this](const sf::Event&) { rotationOffset -= 1; }
	);

	Bind(
		Configuration::PlayerAction::Right,
		[this](const sf::Event&) { rotationOffset += 1; }
	);

	Bind(
		Configuration::PlayerAction::Shoot,
		[this](const sf::Event&) { Shoot(); }
	);

	Bind(
		Configuration::PlayerAction::JumpToHyperspace,
		[this](const sf::Event&) { JumpToHyperspace(); }
	);
}

void Player::ProcessEvents()
{
	isMoving = false;
	rotationOffset = 0;
	TargetForActions::ProcessRealTimeEvents();
}

void Player::Shoot()
{
	if (timeSinceLastShoot > 0.2f)
	{
		world.Add(std::make_unique<PlayerShot>(*this));
		timeSinceLastShoot = 0;
	}
}

void Player::JumpToHyperspace()
{
	moveDirection = sf::Vector2f(0, 0);
	SetPosition(Random::GenerateInt(0, world.GetWidth()), Random::GenerateInt(0, world.GetHeight()));
	world.Add(Configuration::Sound::HyperJump);
}

bool Player::IsCollideWith(const Entity& other) const
{
	// Player can't collide with his shots.
	if (dynamic_cast<const PlayerShot*>(&other) == nullptr)
		return Collision::CheckCircleCollision(sprite, other.sprite);

	return false;
}

void Player::Update(float deltaTime)
{
	timeSinceLastShoot += deltaTime;

	if (rotationOffset != 0)
	{
		float rotationSpeed = 300.0f;
		float angleInDegrees = rotationOffset * rotationSpeed * deltaTime;
		sprite.rotate(angleInDegrees);
	}

	if (isMoving)
	{
		// Convert degrees to radians and subtract 90 degrees (in radians):
		float angleInRadians = sprite.getRotation() / 180 * Math::PI - Math::PI / 2;

		// Convert the angle from polar coordinates to Cartesian coordinates, and
		// set the result to moveSpeed:
		moveDirection += sf::Vector2f(std::cos(angleInRadians), std::sin(angleInRadians)) * moveSpeed * deltaTime;
	}

	sprite.move(moveDirection * deltaTime);
}

void Player::OnDestroy()
{
	Entity::OnDestroy();

	Configuration::player = nullptr;
	Configuration::playerLives--;

	world.Add(Configuration::Sound::PlayerShipExplosion);
}