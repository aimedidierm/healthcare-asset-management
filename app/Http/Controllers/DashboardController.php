<?php

namespace App\Http\Controllers;

use App\Models\Trip;

class DashboardController extends Controller
{
    public function admin()
    {
        return view('admin.dashboard');
    }
}
