@extends('layouts.layout')

@section('content')
<x-admin-nav />
<div class="p-4 sm:ml-64">
    <div class="p-4 border-2 border-gray-200 border-dashed rounded-lg dark:border-gray-700">
        <h5 class="mb-2 text-2xl font-bold tracking-tight text-gray-900">User Dashboard</h5>
        <div class="max-w-full mx-auto bg-white rounded p-6 shadow-md">
            <div class="mb-4">
                <h6 class="text-lg font-semibold text-gray-700">Temperature:</h6>
                <p class="text-xl text-gray-900">{{$sensorData->temperature ?? 'N/A'}}°C </p>
            </div>

            <div>
                <h6 class="text-lg font-semibold text-gray-700">Humidity:</h6>
                <p class="text-xl text-gray-900">{{$sensorData->humidity?? 'N/A'}}%</p>
            </div>
        </div>
    </div>
</div>
@stop