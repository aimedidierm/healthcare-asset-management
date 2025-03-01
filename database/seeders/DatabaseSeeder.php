<?php

namespace Database\Seeders;

// use Illuminate\Database\Console\Seeds\WithoutModelEvents;

use App\Enums\UserRole;
use Illuminate\Database\Seeder;

class DatabaseSeeder extends Seeder
{
    /**
     * Seed the application's database.
     */
    public function run(): void
    {
        \App\Models\User::create([
            'name' => 'Default Admin User',
            'email' => 'admin@example.com',
            'password' => bcrypt('password'),
            'role' => UserRole::ADMIN->value,
        ]);
    }
}
