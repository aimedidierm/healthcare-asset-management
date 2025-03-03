<?php

use App\Http\Controllers\AuthController;
use App\Http\Controllers\BusController;
use App\Http\Controllers\DashboardController;
use App\Http\Controllers\ItemController;
use App\Http\Controllers\PaymentController;
use App\Http\Controllers\StockMovementController;
use App\Http\Controllers\TripController;
use App\Http\Controllers\UserController;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "web" middleware group. Make something great!
|
*/

Route::view('/', 'auth.login')->name('login');
ROute::post('/', [AuthController::class, 'login']);
Route::get('/logout', [AuthController::class, 'logout'])->middleware('auth');

Route::group(["prefix" => "admin", "middleware" => ["auth", "adminCheck"], "as" => "admin."], function () {
    Route::get('/', [DashboardController::class, 'admin']);
    Route::resource('/users', UserController::class)->only('index');
    Route::put('/users', [UserController::class, 'updateSingleUser']);
    Route::view('/settings', 'admin.settings');
    Route::put('/settings', [UserController::class, 'update']);
    Route::resource('/items', ItemController::class)->only('index', 'store', 'destroy');
    Route::get('/inventory/all', [StockMovementController::class, 'all']);
    Route::get('/inventory/in', [StockMovementController::class, 'in']);
    Route::get('/inventory/out', [StockMovementController::class, 'out']);
});
