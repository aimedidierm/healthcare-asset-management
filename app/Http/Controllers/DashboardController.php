<?php

namespace App\Http\Controllers;

use App\Models\Hardware;
use App\Models\Trip;

class DashboardController extends Controller
{
    public function admin()
    {
        $sensorData = Hardware::latest()->first();
        return view('admin.dashboard', compact('sensorData'));
    }
}
